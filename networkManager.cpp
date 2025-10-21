#include "networkManager.h"
#include <QDebug>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {}

/**
 * WLANs scannen – funktioniert auf Raspberry Pi mit wpa_supplicant/iwlist
 */
void NetworkManager::scanNetworks()
{
    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, [this, proc](int, QProcess::ExitStatus) {
        QString output = proc->readAllStandardOutput();
        QStringList ssids = parseIwlistOutput(output);
        emit scanFinished(ssids);
        proc->deleteLater();
    });

    proc->start("sh", QStringList() << "-c" << "iwlist wlan0 scan 2>/dev/null | grep 'ESSID'");
}

/**
 * iwlist-Ausgabe nach SSIDs durchsuchen
 */
QStringList NetworkManager::parseIwlistOutput(const QString &output)
{
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    QStringList ssids;

    QRegularExpression re("ESSID:\"(.*)\"");
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
 * Verbindung konfigurieren:
 * - Bestehende wpa_supplicant.conf in /data anpassen
 * - wpa_supplicant Dienst neu starten
 */
void NetworkManager::connectToNetwork(const QString &ssid, const QString &password)
{
    emit logMessage("Verbinde mit Netzwerk: " + ssid);

    QString confPath = "/data/wpa_supplicant.conf";
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
            // IP über DHCP holen
            QProcess::execute("dhclient wlan0");
        }

        proc->deleteLater();
    });

    // Versuche, über systemd den Dienst neu zu starten (empfohlen)
    proc->start("sh", QStringList() << "-c" << "systemctl restart wpa_supplicant || (killall wpa_supplicant; wpa_supplicant -B -i wlan0 -c /data/wpa_supplicant.conf)");
}
