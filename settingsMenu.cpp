#include "settingsMenu.h"

QPushButton *btnRotation;
QPushButton *btnBrightness;
QPushButton *btnLanguage;
QPushButton *btnSSH;
QPushButton *btnBack;
QPushButton *btnKeyboard;

// --- Hilfsfunktion: sysfs-Wert lesen ---
static int readSysfsValue(const QString &path, int defaultValue = 10) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll().trimmed();
        bool ok = false;
        int value = data.toInt(&ok);
        return ok ? value : defaultValue;
    }
    return defaultValue;
}


// Rotation-Dialog

QDialog* createRotationDialog(QWidget *parent = nullptr) {
    QDialog *dialog = new QDialog(parent);
    dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dialog->setModal(true);
    dialog->setStyleSheet("background-color: #3B4252; color: #ECEFF4; font-size: 18px;");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(20,20,20,20);

    QLabel *label = new QLabel("Display Rotation auswählen:", dialog);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    QComboBox *combo = new QComboBox(dialog);
    combo->addItem("Landscape 0°", 0);
    combo->addItem("Portrait 90°", 1);
    combo->addItem("Landscape 180°", 2);
    combo->addItem("Portrait 270°", 3);
    layout->addWidget(combo);
    combo->setFocus();

    QPushButton *btnOk = new QPushButton("OK", dialog);
    btnOk->setFixedHeight(40);
    layout->addWidget(btnOk);

    auto saveAndClose = [=]() {
        int rotationValue = combo->currentData().toInt();

        QFile configFile("/boot/config.uEnv");
        if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString content = configFile.readAll();
            configFile.close();

            QRegularExpression rx("^rotation=.*$");
            content.replace(rx, QString("rotation=%1").arg(rotationValue));

            if (configFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                QTextStream out(&configFile);
                out << content;
                configFile.close();
            } else {
                qWarning() << "Konnte /boot/config.uEnv nicht schreiben";
            }
        }

        QFile fbconFile("/sys/class/graphics/fbcon/rotate_all");
        if (fbconFile.open(QIODevice::WriteOnly)) {
            QTextStream out(&fbconFile);
            out << rotationValue;
            fbconFile.close();
        } else {
            qWarning() << "Konnte /sys/class/graphics/fbcon/rotate_all nicht schreiben";
        }

        dialog->accept();
    };

    QObject::connect(btnOk, &QPushButton::clicked, saveAndClose);
    // nach Erstellen von combo und dialog:
    auto *filter = new EnterEventFilter(saveAndClose, dialog);

    // Filter auf Dialog und ComboBox setzen
    dialog->installEventFilter(filter);
    combo->installEventFilter(filter);


    dialog->setLayout(layout);
    return dialog;
}

// --- Dialog für Display-Helligkeit ---
QDialog* createBrightnessDialog(QWidget *parent = nullptr) {
	QDialog *dialog = new QDialog(parent);
	dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	dialog->setModal(true);
	dialog->setStyleSheet("background-color: #3B4252; color: #ECEFF4; font-size: 18px;");

	QVBoxLayout *layout = new QVBoxLayout(dialog);
	layout->setContentsMargins(20,20,20,20);

	// Titel
	QLabel *label = new QLabel("Display Helligkeit", dialog);
	label->setAlignment(Qt::AlignCenter);
	layout->addWidget(label);

	// Slider
	QSlider *slider = new QSlider(Qt::Horizontal, dialog);
	slider->setTickPosition(QSlider::TicksBelow);

	int maxBrightness = readSysfsValue("/sys/class/backlight/lcd/max_brightness", 10);
	int currentBrightness = readSysfsValue("/sys/class/backlight/lcd/brightness", maxBrightness / 2);
	slider->setRange(1, maxBrightness);
	slider->setValue(currentBrightness);
	layout->addWidget(slider);

	// Anzeige aktueller Wert
	QLabel *valueLabel = new QLabel(QString("Stufe %1").arg(currentBrightness), dialog);
	valueLabel->setAlignment(Qt::AlignCenter);
	layout->addWidget(valueLabel);

	// Slider-Event: Wert in sysfs schreiben
	QObject::connect(slider, &QSlider::valueChanged, [=](int value){
		valueLabel->setText(QString("Stufe %1").arg(value));

		QFile file("/sys/class/backlight/lcd/brightness");
		if (file.open(QIODevice::WriteOnly)) {
			QTextStream out(&file);
			out << value;
			file.close();
		} else {
			qWarning() << "Konnte Helligkeit nicht setzen – evtl. fehlen Rechte?";
		}
	});

	// OK-Button
	QPushButton *btnOk = new QPushButton("OK", dialog);
	btnOk->setFixedHeight(40);
	QObject::connect(btnOk, &QPushButton::clicked, dialog, &QDialog::accept);
	layout->addWidget(btnOk);

	dialog->setLayout(layout);
	return dialog;
}

void retranslateAllWidgets() {

    btnRotation->setText(QObject::tr("Display Rotation"));
	btnBrightness->setText(QObject::tr("Display Helligkeit"));
	btnLanguage->setText(QObject::tr("Sprache"));
	btnSSH->setText(QObject::tr("SSH"));
	btnKeyboard->setText(QObject::tr("Tastatur"));
	btnBack->setText(QObject::tr("Zurück"));

    // usw. für alle Buttons und Labels
}

// --- Language Dialog ---
QDialog* createLanguageDialog(QWidget *parent, QTranslator *translator) {
    QDialog *dialog = new QDialog(parent);
    dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QPushButton *btnGerman = new QPushButton(QObject::tr("Deutsch"), dialog);
    QPushButton *btnEnglish = new QPushButton(QObject::tr("English"), dialog);

    layout->addWidget(btnGerman);
    layout->addWidget(btnEnglish);

    QObject::connect(btnGerman, &QPushButton::clicked, [=]()
    {
        LanguageHelper::instance()->changeLanguage("flyberry_de");
        dialog->accept();
    });

    QObject::connect(btnEnglish, &QPushButton::clicked, [=]() {
    	LanguageHelper::instance()->changeLanguage("flyberry_en");
        dialog->accept();
    });

    return dialog;
}


// --- Hilfsdialog für Tastaturlayout ---
QDialog* createKeyboardDialog(QWidget *parent = nullptr) {
    QDialog *dialog = new QDialog(parent);
    dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dialog->setModal(true);
    dialog->setStyleSheet("background-color: #3B4252; color: #ECEFF4; font-size: 18px;");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(20,20,20,20);

    QLabel *label = new QLabel(QObject::tr("Tastatur-Layout auswählen:"), dialog);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    QComboBox *combo = new QComboBox(dialog);
    // ein paar gängige Layouts
    combo->addItem("Deutsch (de)", "de");
    combo->addItem("Englisch US (us)", "us");
    combo->addItem("Englisch UK (uk)", "uk");
    combo->addItem("Französisch (fr)", "fr");
    combo->addItem("Spanisch (es)", "es");
    layout->addWidget(combo);

    QPushButton *btnOk = new QPushButton(QObject::tr("OK"), dialog);
    btnOk->setFixedHeight(40);
    layout->addWidget(btnOk);

    auto saveAndClose = [=]() {

	   QString layoutCode = combo->currentData().toString();
		if(!layoutCode.isEmpty()) {
		  QProcess::execute("localectl", {"set-keymap", layoutCode});
		}
		dialog->accept();
	};
    QObject::connect(btnOk, &QPushButton::clicked, saveAndClose);
	// nach Erstellen von combo und dialog:
	auto *filter = new EnterEventFilter(saveAndClose, dialog);

	// Filter auf Dialog und ComboBox setzen
	dialog->installEventFilter(filter);
	combo->installEventFilter(filter);

    dialog->setLayout(layout);
    return dialog;
}


// --- Settings-Menü ---
QWidget* createSettingsMenu(QStackedWidget *stack, QWidget *systemMenu)
{
    QWidget *menu = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(menu);
    layout->setSpacing(5);
    layout->setContentsMargins(5,5,5,5);

    // Buttons

     btnRotation   = createMenuButton(QObject::tr("Display Rotation"));
     btnBrightness = createMenuButton(QObject::tr("Display Helligkeit"));
     btnLanguage   = createMenuButton(QObject::tr("Sprache"));
     btnSSH        = createMenuButton(QObject::tr("SSH"));
     btnKeyboard   = createMenuButton(QObject::tr("Tastatur"));
     btnBack       = createMenuButton(QObject::tr("Zurück"));


    layout->addWidget(btnRotation);
    layout->addWidget(btnBrightness);
    layout->addWidget(btnLanguage);
    layout->addWidget(btnSSH);
    layout->addWidget(btnKeyboard);
    layout->addWidget(btnBack);

    layout->addStretch();

    // --- Aktionen ---
    QObject::connect(btnRotation, &QPushButton::clicked, [=]() {
		QDialog *dialog = createRotationDialog(menu);
		dialog->exec();
	});

    QObject::connect(btnBrightness, &QPushButton::clicked, [=]() {
        QDialog *dialog = createBrightnessDialog(menu);
        dialog->exec();
    });

    static QTranslator *translator = new QTranslator();
    QObject::connect(btnLanguage, &QPushButton::clicked, [=]() {
        QDialog *dialog = createLanguageDialog(menu, translator);
        dialog->exec();
        delete dialog;
    });

    QObject::connect(btnKeyboard, &QPushButton::clicked, [=]() {
        QDialog *dialog = createKeyboardDialog(menu);
        dialog->exec();
        delete dialog;
    });

    QObject::connect(btnBack, &QPushButton::clicked, [=]() {
        stack->setCurrentWidget(systemMenu);
    });

    // Language update
    QObject::connect(LanguageHelper::instance(), &LanguageHelper::languageChanged, &retranslateAllWidgets);
    return menu;
}
