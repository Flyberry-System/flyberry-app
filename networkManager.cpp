#include "networkManager.h"
#include <QDebug>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <spdlog/spdlog.h>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {}

/**
 * WLANs scannen – funktioniert auf Raspberry Pi mit 'iw'
 */
void NetworkManager::scanNetworks()
{
    QString cmd;

    spdlog::info("Network Manager scan networks");


    QFile cpuInfo("/proc/cpuinfo");
    bool isRaspberryPi = false;
   

    if (isRaspberryPi) 
    {
        cmd = "iw wlan0 scan | grep 'SSID:'";
    } 
    else 
    {
        cmd = "nmcli -t -f SSID dev wifi";
    }

  
    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, [this, proc, isRaspberryPi](int, QProcess::ExitStatus) {
        QString output = proc->readAllStandardOutput();
        QStringList ssids;

        if (isRaspberryPi) 
        {
            for (const QString &line : output.split('\n', Qt::SkipEmptyParts)) 
            {
                QString ssid = line.section("SSID:", 1).trimmed();
                if (!ssid.isEmpty())
                    ssids << ssid;
            }
        } else {
            ssids << "HomeWiFi" << "OfficeNet" << "GuestNetwork" << "TestNetwork";
           // ssids = output.split('\n', Qt::SkipEmptyParts);
        }

        emit scanFinished(ssids);
        proc->deleteLater();
    });

    proc->start("sh", QStringList() << "-c" << cmd);
}

/**
 * iw-Ausgabe nach SSIDs durchsuchen
 */
QStringList NetworkManager::parseIwlistOutput(const QString &output)
{
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    QStringList ssids;

    QRegularExpression re("SSID:\\s*(.*)");
    for (const QString &line : lines) {
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString ssid = match.captured(1).trimmed();
            if (!ssid.isEmpty() && ssid != "hidden")
                ssids << ssid;
        }
    }
    ssids.removeDuplicates();
    return ssids;
}

/**
 * Verbindung konfigurieren und herstellen
 */
void NetworkManager::connectToNetwork(const QString &ssid, const QString &password)
{
    spdlog::info("Connecting to network: {}", ssid.toStdString());

    QProcess *proc = new QProcess(this);

    connect(proc, &QProcess::finished,
            [this, proc](int exitCode, QProcess::ExitStatus) {

        QString output = proc->readAllStandardOutput();
        QString error  = proc->readAllStandardError();

        bool success = (exitCode == 0);

        if (success) {
            spdlog::info("nmcli success: {}", output.toStdString());
            emit connectionStatus("Mit WLAN verbunden.", true);
        } else {
            spdlog::error("nmcli failed: {}", error.toStdString());
            emit connectionStatus("Verbindung fehlgeschlagen.", false);
        }

        proc->deleteLater();
    });

    // nmcli Befehl
    QStringList args;
    args << "dev" << "wifi" << "connect" << ssid << "password" << password;

    proc->start("nmcli", args);
}