#include "networkMenu.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidget>
#include <QProcess>
#include <QStackedWidget>
#include <QStringList>
#include <QString>
#include "menuButton.h"

QWidget* createNetworkMenu(QStackedWidget *stack, QWidget *systemMenu)
{
    QWidget *networkMenu = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(networkMenu);
    layout->setSpacing(5);
    layout->setContentsMargins(5,5,5,5);

    // Liste für SSIDs
    QListWidget *ssidList = new QListWidget();
    layout->addWidget(ssidList);

    // Netzwerk scannen Button
    QPushButton *btnScan = createMenuButton("Netzwerk scannen");
    layout->addWidget(btnScan);

    QObject::connect(btnScan, &QPushButton::clicked, [=]() {
        ssidList->clear();
        QProcess process;
        process.start("sh", QStringList() << "-c" << "sudo iwlist wlan0 scan | grep 'ESSID'");
        process.waitForFinished();
        QString output = process.readAllStandardOutput();
        QStringList ssids = output.split('\n', Qt::SkipEmptyParts);
        ssidList->addItems(ssids);
    });

    // Zurück-Button
    QPushButton *btnBack = createMenuButton("Zurück");
    QObject::connect(btnBack, &QPushButton::clicked, [=]() {
        stack->setCurrentWidget(systemMenu);
    });
    layout->addWidget(btnBack);

    layout->addStretch();
    networkMenu->setLayout(layout);

    return networkMenu;
}

