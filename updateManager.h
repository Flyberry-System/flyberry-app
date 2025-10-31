#pragma once
#include <QObject>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class UpdateManager : public QObject {
    Q_OBJECT

public:
    explicit UpdateManager(QObject *parent = nullptr);

public slots:
    void checkForUpdates();
    void installUpdate(const QString &bundlePath);
    bool checkServerReachable(const QString &host, quint16 port, int timeoutMs);

signals:
    void updateAvailable(const QString &version);
    void updateNotAvailable();
    void updateProgress(int percent);
    void updateFinished(bool success);
    void logMessage(const QString &msg);


private slots:
    void onCheckReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
};
