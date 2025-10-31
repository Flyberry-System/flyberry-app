#include "updateMenu.h"
#include <QVBoxLayout>
#include <QProcess>
#include <QLabel>
#include <QRegularExpression>
#include <QScrollBar>

UpdateMenu::UpdateMenu(QStackedWidget *stack_, QWidget *systemMenu_, QWidget *parent)
    : QWidget(parent), stack(stack_), systemMenu(systemMenu_)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(5);
	layout->setContentsMargins(5, 5, 5, 5);

	QLabel *title = new QLabel(tr("System-Update"));
	title->setStyleSheet("font-size: 20px; font-weight: bold;");
	layout->addWidget(title);

	lblActiveSlot = new QLabel("Aktiver Slot: –");
	lblVersion    = new QLabel("Version: –");
	lblStatus     = new QLabel("Status: –");
	layout->addWidget(lblActiveSlot);
	layout->addWidget(lblVersion);
	layout->addWidget(lblStatus);

	QPushButton *btnCheck   = createMenuButton(tr("Nach Update suchen"));
	btnStartUpdate          = createMenuButton(tr("Update starten"));
//	btnStartUpdate->setEnabled(false);

	logArea = new QTextEdit();
	logArea->setReadOnly(true);
	logArea->setMinimumHeight(200);

	QPushButton *btnBack = createMenuButton(tr("Zurück"));

	layout->addWidget(btnCheck);
	layout->addWidget(btnStartUpdate);
	layout->addWidget(btnBack);
//	layout->addWidget(logArea);

	layout->addStretch();

	setLayout(layout);

	connect(btnCheck, &QPushButton::clicked, this, &UpdateMenu::checkForUpdate);
	connect(btnStartUpdate, &QPushButton::clicked, this, &UpdateMenu::startUpdate);
	connect(btnBack, &QPushButton::clicked, [=]() {
		stack->setCurrentWidget(systemMenu);
	});

	// Liste aller Buttons speichern für Navigation
//		buttons = {btnRefresh, btnCheck, btnStartUpdate, btnBack};

	// Fokus auf den ersten Button setzen
	if (!buttons.isEmpty())
		buttons.first()->setFocus();

	refreshStatus();
	updatemanager = new UpdateManager(this);
}


/**
 * Keyboard Event Handling für Pfeiltasten-Navigation
 */
void UpdateMenu::keyPressEvent(QKeyEvent *event)
{
//    if (buttons.isEmpty()) return;
//
//    int currentIndex = -1;
//    for (int i = 0; i < buttons.size(); ++i) {
//        if (buttons[i]->hasFocus()) {
//            currentIndex = i;
//            break;
//        }
//    }
//
//    if (event->key() == Qt::Key_Down) {
//        int next = (currentIndex + 1) % buttons.size();
//        buttons[next]->setFocus();
//    }
//    else if (event->key() == Qt::Key_Up) {
//        int prev = (currentIndex - 1 + buttons.size()) % buttons.size();
//        buttons[prev]->setFocus();
//    }
//    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
//        if (currentIndex >= 0)
//            QMetaObject::invokeMethod(buttons[currentIndex], "click");
//    }
//    else {
//        QWidget::keyPressEvent(event);
//    }
}


/**
 * Liest RAUC Statusinformationen (JSON) und zeigt sie an.
 */
void UpdateMenu::refreshStatus()
{
    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, [=](int, QProcess::ExitStatus) {
        QString output = proc->readAllStandardOutput();

        if (output.isEmpty()) {
            logArea->append("⚠️ Keine RAUC-Ausgabe erhalten!");
            proc->deleteLater();
            return;
        }

        parseRaucStatus(output);
        proc->deleteLater();
    });

    // JSON-Format verwenden
    proc->start("sh", QStringList() << "-c" << "rauc status --detailed --output-format=json");
}

/**
 * Parst RAUC JSON-Statusdaten und aktualisiert das UI.
 */
void UpdateMenu::parseRaucStatus(const QString &output)
{
	QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
	if (!doc.isObject()) {
		logArea->append("❌ Fehler: Ungültiges JSON von rauc status.");
		return;
	}

	QJsonObject root = doc.object();

	// JSON-Felder auslesen
	QString booted = root.value("booted").toString();
	QString version = root.value("compatible").toString();

	// ⚠️ WICHTIG: Variablenname geändert, da "slots" durch Qt reserviert ist
	QJsonArray slotArray = root.value("slot-status").toArray();

	QString statusText;
	for (const QJsonValue &val : slotArray) {
		QJsonObject slot = val.toObject();
		QString version = slot.value("version").toString();
		QString state = slot.value("state").toString();
		QString desc = slot.value("description").toString();
		statusText += QString("%1: %2 (%3)\n").arg(version, state, desc);
	}

	lblActiveSlot->setText(QString("Aktiver Slot: %1").arg(booted.isEmpty() ? "---" : booted));
	lblVersion->setText(QString("Version: %1").arg(version.isEmpty() ? "---" : version));
	lblStatus->setText(statusText.isEmpty() ? "Keine Slot-Informationen" : statusText);
}

/**
 * Prüft, ob ein Update verfügbar ist.
 */
void UpdateMenu::checkForUpdate()
{
	StatusDialog *dlg = new StatusDialog(this);
	dlg->centerOnParent();
	dlg->show();

	dlg->appendMessage("Prüfe Verbindung zum Update-Server...");

	bool reachable = updatemanager->checkServerReachable("192.168.178.24", 8000, 1000);

	if (!reachable) {
		dlg->appendMessage("Server nicht erreichbar");
		QTimer::singleShot(1000, dlg, &QDialog::accept);
		return;
	}

	dlg->appendMessage("✅ Server erreichbar, prüfe RAUC-Status...");

	// Prüfen, ob RAUC-Bundle existiert
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	QUrl url("http://192.168.178.24:8000/update-ov-ov-rpi4-64.raucb");
	QNetworkRequest request(url);
	QNetworkReply *reply = manager->head(request);

	connect(reply, &QNetworkReply::finished, [=]() {
		if (reply->error() == QNetworkReply::NoError &&
			reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
		{
			dlg->appendMessage("RAUC-Bundle gefunden");
			// TODO: Update-Verfügbarkeit prüfen, z.B. aktive Version vs Bundle-Version
			dlg->appendMessage("\nUpdate verfügbar!");

			QTimer::singleShot(1000, dlg, &QDialog::accept);

		} else {
			dlg->appendMessage("RAUC-Bundle nicht gefunden!");
			QTimer::singleShot(1000, dlg, &QDialog::accept);
		}
		reply->deleteLater();
	});
}


void UpdateMenu::startUpdate()
{
    StatusDialog *dialog = new StatusDialog(this);
    dialog->appendStatus("🔍 Starte Update-Prozess...");
    dialog->showProgressBar(true); // Fortschrittsbalken aktivieren
    dialog->show();

    QString updateFile = "update-ov-ov-rpi4-64.raucb";
    QString serverUrl = "https://flyberry.de/wp-content/software-update";
    //	QString serverUrl = "http://192.168.178.24:8000/update-ov-ov-rpi4-64.raucb";
    QString localPath = "/tmp";

    // 1️⃣ Bundle mit wget herunterladen
    QProcess *wgetProc = new QProcess(this);
    connect(wgetProc, &QProcess::readyReadStandardOutput, [=]() {
        QString output = QString::fromUtf8(wgetProc->readAllStandardOutput());
        dialog->appendStatus(output);
    });

    connect(wgetProc, &QProcess::readyReadStandardError, [=]() {
        QString errorOut = QString::fromUtf8(wgetProc->readAllStandardError());
        dialog->appendStatus(errorOut);

        // Fortschritt in wget-Ausgabe (BusyBox gibt keine Prozent, daher nur Anzeige)
    });

    connect(wgetProc, &QProcess::finished, [=](int exitCode, QProcess::ExitStatus) {
        if (exitCode == 0) {
            dialog->appendStatus("Download abgeschlossen. Starte Installation...");
            dialog->setProgress(0);

            // 2️⃣ Installation mit RAUC starten
            QProcess *raucProc = new QProcess(dialog);
            connect(raucProc, &QProcess::readyReadStandardOutput, [=]() {
                QString output = QString::fromUtf8(raucProc->readAllStandardOutput());
                dialog->appendStatus(output);

                // Fortschritt aus RAUC-Ausgabe extrahieren: z. B. "[ 40%]"
                QRegularExpression re("\\[\\s*(\\d+)%\\]");
                QRegularExpressionMatch match = re.match(output);
                if (match.hasMatch()) {
                    int percent = match.captured(1).toInt();
                    dialog->setProgress(percent);
                }
            });

            connect(raucProc, &QProcess::readyReadStandardError, [=]() {
                QString errorOut = QString::fromUtf8(raucProc->readAllStandardError());
                dialog->appendStatus(errorOut);

                // Auch STDERR nach Fortschritt durchsuchen (RAUC kann hier schreiben)
                QRegularExpression re("\\[\\s*(\\d+)%\\]");
                QRegularExpressionMatch match = re.match(errorOut);
                if (match.hasMatch()) {
                    int percent = match.captured(1).toInt();
                    dialog->setProgress(percent);
                }
            });

            connect(raucProc, &QProcess::finished, [=](int raucExit, QProcess::ExitStatus) {
                if (raucExit == 0) {
                    dialog->appendStatus("Update erfolgreich installiert! System kann neu gestartet werden.");
                    dialog->setProgress(100);
                } else {
                    dialog->appendStatus("Update fehlgeschlagen.");
                }
                QTimer::singleShot(2000, dialog, &QDialog::accept);
            });

            QString installCmd = QString("rauc install %1/%2").arg(localPath, updateFile);
            raucProc->start("sh", QStringList() << "-c" << installCmd);
        } else {
            dialog->appendStatus("Download fehlgeschlagen.");
            QTimer::singleShot(2000, dialog, &QDialog::accept);
        }

        wgetProc->deleteLater();
    });

    // Start des Downloads
    QString wgetCmd = QString("wget %1/%2 -q -P %3").arg(serverUrl, updateFile, localPath);
    dialog->appendStatus("Lade Update herunter...");
    wgetProc->start("sh", QStringList() << "-c" << wgetCmd);
}
