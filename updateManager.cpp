#include "updateManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QDebug>

UpdateManager::UpdateManager(QObject *parent)
    : QObject(parent), manager(new QNetworkAccessManager(this))
{
    connect(manager, &QNetworkAccessManager::finished, this, &UpdateManager::onCheckReply);
}

void UpdateManager::checkForUpdates()
{
    emit logMessage("Prüfe auf Updates ...");
    QUrl url("https://updates.flyberry-system.de/latest.json"); // Beispiel-URL
    QNetworkRequest request(url);
    manager->get(request);
}

void UpdateManager::onCheckReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit logMessage("Fehler beim Update-Check: " + reply->errorString());
        emit updateNotAvailable();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QString latestVersion = doc["version"].toString();
    QString currentVersion = "1.0.0"; // TODO: aus Datei oder Build-Info lesen

    if (latestVersion != currentVersion) {
        emit updateAvailable(latestVersion);
    } else {
        emit updateNotAvailable();
    }
}

void UpdateManager::installUpdate(const QString &bundlePath)
{
    emit logMessage("Starte RAUC-Update von " + bundlePath);

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::readyReadStandardOutput, [this, proc]() {
        emit logMessage(proc->readAllStandardOutput());
    });
    connect(proc, &QProcess::readyReadStandardError, [this, proc]() {
        emit logMessage(proc->readAllStandardError());
    });
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, proc](int exitCode, QProcess::ExitStatus) {
                bool success = (exitCode == 0);
                emit updateFinished(success);
                proc->deleteLater();
            });

    // RAUC installieren
    proc->start("rauc", QStringList() << "install" << bundlePath);
}
