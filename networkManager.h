#pragma once
#include <QObject>
#include <QStringList>
#include <QProcess>

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);

public slots:
    void scanNetworks();
    void connectToNetwork(const QString &ssid, const QString &password);

signals:
    void scanFinished(const QStringList &ssids);
    void connectionStatus(const QString &message, bool success);
    void logMessage(const QString &msg);

private:
    QStringList parseIwlistOutput(const QString &output);
};
