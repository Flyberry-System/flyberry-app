#include "updateMenu.h"

#include <QVBoxLayout>
#include <QProcess>
#include <QLabel>
#include <QRegularExpression>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextEdit>

UpdateMenu::UpdateMenu(QStackedWidget *stack_, QWidget *systemMenu_, QWidget *parent) : QWidget(parent), stack(stack_), systemMenu(systemMenu_)
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
    
    btnCheck       = createMenuButton(tr("Nach Update suchen"));
    btnStartUpdate = createMenuButton(tr("Update starten"));
    btnStartUpdate->setEnabled(false);

    logArea = new QTextEdit();
    logArea->setReadOnly(true);
    logArea->setMinimumHeight(200);

    QPushButton *btnBack = createMenuButton(tr("Zurück"));

    layout->addWidget(btnCheck);
    layout->addWidget(btnStartUpdate);
    layout->addWidget(btnBack);

    layout->addStretch();

    connect(btnCheck, &QPushButton::clicked,
            this, &UpdateMenu::checkForUpdate);

    connect(btnStartUpdate, &QPushButton::clicked,
            this, &UpdateMenu::startUpdate);

    connect(btnBack, &QPushButton::clicked, [=]() {
        stack->setCurrentWidget(systemMenu);
    });

    updatemanager = new UpdateManager(this);

    refreshStatus();
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
    auto *proc = new QProcess(this);
    proc->setProgram("rauc");
    proc->setArguments({"status", "--detailed", "--output-format=json"});
    proc->setProcessChannelMode(QProcess::MergedChannels);

    connect(proc, &QProcess::finished, this, [=](int, QProcess::ExitStatus) {
        QString output = proc->readAllStandardOutput();

        if (!output.isEmpty())
            parseRaucStatus(output);
        else
            logArea->append("⚠️ Keine RAUC-Ausgabe erhalten");

        proc->deleteLater();
    });

    proc->start();
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

    dlg->appendMessage("Prüfe Server...");

    if (!updatemanager->checkServerReachable("192.168.178.24", 8000, 1000)) {
        dlg->appendMessage("❌ Server nicht erreichbar");
        QTimer::singleShot(1000, dlg, &QDialog::accept);
        return;
    }

    dlg->appendMessage("✅ Server erreichbar");

    auto *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("http://192.168.178.24:8000/update.raucb"));

    QNetworkReply *reply = manager->head(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() == QNetworkReply::NoError &&
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
        {
            dlg->appendMessage("Update verfügbar");
            btnStartUpdate->setEnabled(true);
        }
        else {
            dlg->appendMessage("Kein Update gefunden");
        }

        QTimer::singleShot(1000, dlg, &QDialog::accept);
        reply->deleteLater();
    });
}


void UpdateMenu::startUpdate()
{
    StatusDialog *dialog = new StatusDialog(this);
    dialog->showProgressBar(true);
    dialog->show();

    QString url = "https://flyberry.de/wp-content/software-update/update-ov-ov-rpi4-64.raucb";
    QString filePath = "/tmp/update.raucb";

    dialog->appendStatus("Download startet...");

    // DOWNLOAD
    QProcess *wget = new QProcess(this);
    wget->setProgram("wget");
    wget->setArguments({url, "-O", filePath});

    connect(wget, &QProcess::readyReadStandardOutput, this, [=]() {
        dialog->appendStatus(wget->readAllStandardOutput());
    });

    connect(wget, &QProcess::finished, this, [=](int code) {
        if (code != 0) {
            dialog->appendStatus("❌ Download fehlgeschlagen");
            dialog->close();
            return;
        }

        dialog->appendStatus("Download fertig. Installiere...");

        // INSTALL
        QProcess *rauc = new QProcess(this);
        rauc->setProgram("rauc");
        rauc->setArguments({"install", filePath});

        static const QRegularExpression re(R"(\[\s*(\d+)%\])");

        connect(rauc, &QProcess::readyReadStandardOutput, this, [=]() {
            QString out = rauc->readAllStandardOutput();
            dialog->appendStatus(out);

            auto m = re.match(out);
            if (m.hasMatch())
                dialog->setProgress(m.captured(1).toInt());
        });

        connect(rauc, &QProcess::finished, this, [=](int exitCode) {
            if (exitCode == 0) {
                dialog->appendStatus("✔ Update erfolgreich");
                dialog->setProgress(100);
            } else {
                dialog->appendStatus("❌ Update fehlgeschlagen");
            }

            QTimer::singleShot(2000, dialog, &QDialog::accept);
        });

        rauc->start();
    });

    wget->start();
}
