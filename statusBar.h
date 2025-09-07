#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QFrame>
#include <QLabel>
#include <QTimer>

class StatusBar : public QFrame {
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = nullptr);

private slots:
    void updateStatus();

private:
    QLabel *wifiIcon;
    QLabel *timeLabel;
    QLabel *voltageIcon;
    QTimer *timer;
    QLabel *voltageLabel;
};

#endif // STATUSBAR_H
