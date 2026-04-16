#include "networkManager.h"
#include <QDebug>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {}

/**
 * WLANs scannen – funktioniert auf Raspberry Pi mit 'iw'
 */
void NetworkManager::scanNetworks()
{
    QString cmd;

    QFile cpuInfo("/proc/cpuinfo");
    bool isRaspberryPi = true;
   

    if (isRaspberryPi) {
        cmd = "iw wlan0 scan | grep 'SSID:'";
    } else {
        cmd = "nmcli -t -f SSID dev wifi";
    }

  
    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, [this, proc, isRaspberryPi](int, QProcess::ExitStatus) {
        QString output = proc->readAllStandardOutput();
        QStringList ssids;

        if (isRaspberryPi) {
            for (const QString &line : output.split('\n', Qt::SkipEmptyParts)) {
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
    emit logMessage("Verbinde mit Netzwerk: " + ssid);

    QString confPath = "/data/wpa_supplicant/wpa_supplicant-wlan0.conf";
    QFile file(confPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit connectionStatus("Fehler: Kann /data/wpa_supplicant.conf nicht schreiben", false);
        return;
    }

    QTextStream out(&file);
    out << "ctrl_interface=/var/run/wpa_supplicant\n";
    out << "update_config=1\n";
    out << "country=DE\n\n";
    out << "network={\n";
    out << "    ssid=\"" << ssid << "\"\n";
    out << "    psk=\"" << password << "\"\n";
    out << "    key_mgmt=WPA-PSK\n";
    out << "}\n";
    file.close();

    // Dienst neu starten, um Konfiguration zu übernehmen
    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool success = (exitCode == 0);
        QString msg = success ? "Mit WLAN verbunden (oder Verbindung wird aufgebaut)." : "Verbindung fehlgeschlagen.";
        emit connectionStatus(msg, success);

        if (success) {
            QProcess::execute("dhclient wlan0");
        }

        proc->deleteLater();
    });

    // Versuche über systemd neu zu starten, sonst manuell
    proc->start("sh", QStringList() << "-c" <<
        "systemctl restart wpa_supplicant || "
        "(killall wpa_supplicant; "
        "wpa_supplicant -B -i wlan0 -c /data/wpa_supplicant/wpa_supplicant-wlan0.conf)");
}
