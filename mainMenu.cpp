// MainMenu.cpp
#include "mainMenu.h"


MainMenu::MainMenu(QStackedWidget* stack_, QWidget* systemMenu_, QWidget* parent)
    : QWidget(parent), stack(stack_), systemMenu(systemMenu_)
{
	// --- Scale-Faktor automatisch ermitteln ---
	QScreen *screen = QGuiApplication::primaryScreen();
	qreal dpi = screen ? screen->logicalDotsPerInch() : 96.0;
	qreal scaleFactor = dpi / 96.0;  // 96 DPI = Standard
	if (scaleFactor < 1.0) scaleFactor =  1.0; // Minimum

	QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);
    layout->setContentsMargins(15,15,15,15);

    btnXCSoar   = createMenuButton(tr("Starte XCSoar"));
    btnSystem   = createMenuButton(tr("System") );
    btnShell    = createMenuButton(tr("Shell"));
    btnReboot   = createMenuButton(tr("Neustart"));
    btnShutdown = createMenuButton(tr("Ausschalten"));

    QList<QPushButton*> buttons = {btnXCSoar, btnSystem, btnShell, btnReboot, btnShutdown};

       for (auto *btn : buttons) {
           btn->setMinimumHeight(60 * scaleFactor); // Höhe proportional
           QFont f = btn->font();
           f.setPointSizeF(f.pointSizeF() * scaleFactor); // Schrift skalieren
           btn->setFont(f);
       }

    layout->addWidget(btnXCSoar);
    layout->addWidget(btnSystem);
    layout->addWidget(btnShell);
    layout->addWidget(btnReboot);
    layout->addWidget(btnShutdown);
    layout->addStretch();

    // Aktionen
    QObject::connect(btnXCSoar, &QPushButton::clicked, [=]() {
        QProcess::startDetached("/usr/bin/xcsoar", QStringList() << "-fly" << "-fullscreen");
    });

    QObject::connect(btnSystem, &QPushButton::clicked, [=]() {
        stack->setCurrentWidget(systemMenu);
    });

    QObject::connect(btnShell, &QPushButton::clicked, [=]() {
        QProcess::startDetached("/usr/bin/matchbox-terminal");
    });

//    QObject::connect(btnReboot, &QPushButton::clicked, []() {
//        QProcess::startDetached("reboot");
//    });
//
//    QObject::connect(btnShutdown, &QPushButton::clicked, []() {
//        QProcess::startDetached("poweroff");
//    });


    QObject::connect(btnReboot, &QPushButton::clicked, [=]() {
        ConfirmDialog dlg("Möchtest du das System wirklich neu starten?");
        if (dlg.exec() == QDialog::Accepted)
            QProcess::startDetached("reboot");
    });

    QObject::connect(btnShutdown, &QPushButton::clicked, [=]() {
        ConfirmDialog dlg("Möchtest du das System wirklich ausschalten?");
        if (dlg.exec() == QDialog::Accepted)
            QProcess::startDetached("poweroff");
    });

    // Language update
    connect(LanguageHelper::instance(), &LanguageHelper::languageChanged, this, &MainMenu::retranslateUi);
}

void MainMenu::retranslateUi() {
    btnXCSoar->setText(tr("Starte XCSoar"));
    btnSystem->setText(tr("System"));
    btnShell->setText(tr("Shell"));
    btnReboot->setText(tr("Neustart"));
    btnShutdown->setText(tr("Ausschalten"));
}
