#include "statusBar.h"
#include <QHBoxLayout>
#include <QPixmap>
#include <QTime>
#include <QFile>

StatusBar::StatusBar(QWidget *parent) : QFrame(parent)
{
    setFrameShape(QFrame::NoFrame);
    setFixedHeight(30);
    setStyleSheet("background-color: #4C566A; color: #ECEFF4; font-size: 16px;");

    wifiIcon = new QLabel(this);
    wifiIcon->setPixmap(QPixmap("icons/wifi.svg").scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    timeLabel = new QLabel("--:--:--", this);

    voltageIcon = new QLabel(this);
    voltageIcon->setPixmap(QPixmap("icons/battery.svg").scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    voltageLabel = new QLabel("-- V", this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10,0,10,0);
    layout->addWidget(wifiIcon, 0, Qt::AlignLeft);
    layout->addStretch();
    layout->addWidget(timeLabel, 0, Qt::AlignCenter);
    layout->addStretch();



	QWidget *batteryWidget = new QWidget(this);
	QHBoxLayout *batteryLayout = new QHBoxLayout(batteryWidget);
	batteryLayout->setContentsMargins(0,0,0,0);
	batteryLayout->setSpacing(5);
	batteryLayout->addWidget(voltageIcon);
	batteryLayout->addWidget(voltageLabel);
	layout->addWidget(batteryWidget, 0, Qt::AlignRight);

        setLayout(layout);
    // Timer für Updates
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &StatusBar::updateStatus);
    timer->start(1000); // jede Sekunde
}

void StatusBar::updateStatus()
{
    // --- Uhrzeit aktualisieren ---
    timeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));

    // --- Spannung aus /sys/class/hwmon/hwmon2/in1_input lesen ---
    double voltage = 0.0;
    QFile file("/sys/class/hwmon/hwmon2/in1_input");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        voltage = line.toDouble() / 1000.0;  // Sysfs liefert mV, wir wollen V
        file.close();
    } else {
        qWarning() << "Kann /sys/class/hwmon/hwmon2/in1_input nicht lesen!";
    }

    // --- Spannungsanzeige aktualisieren ---
    voltageLabel->setText(QString("%1 V").arg(voltage, 0, 'f', 1));
}
