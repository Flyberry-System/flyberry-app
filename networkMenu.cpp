#include "networkMenu.h"
#include "menuButton.h"
#include "networkManager.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QStackedWidget>
#include <QMessageBox>

QWidget* createNetworkMenu(QStackedWidget *stack, QWidget *systemMenu)
{
    QWidget *networkMenu = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(networkMenu);
    layout->setSpacing(5);
    layout->setContentsMargins(10, 10, 10, 10);

    // Überschrift
    QLabel *lblTitle = new QLabel(QObject::tr("WLAN-Verbindungen"));
    lblTitle->setAlignment(Qt::AlignCenter);
    lblTitle->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(lblTitle);

    // Statusanzeige
    QLabel *lblStatus = new QLabel(QObject::tr("Bereit"));
    lblStatus->setAlignment(Qt::AlignCenter);
    lblStatus->setStyleSheet("color: gray;");
    layout->addWidget(lblStatus);

    // Liste mit verfügbaren Netzwerken
    QListWidget *ssidList = new QListWidget();
    ssidList->setStyleSheet("font-size: 16px;");
    layout->addWidget(ssidList, 1);

    // Buttons
    QPushButton *btnScan = createMenuButton(QObject::tr("Netzwerke scannen"));
    QPushButton *btnConnect = createMenuButton(QObject::tr("Verbinden"));
    QPushButton *btnBack = createMenuButton(QObject::tr("Zurück"));
    layout->addWidget(btnScan);
    layout->addWidget(btnConnect);
    layout->addWidget(btnBack);

    layout->addStretch();

    // Netzwerkmanager-Instanz
    auto *manager = new NetworkManager(networkMenu);

    // --- Verbindungen herstellen ---
    QObject::connect(btnScan, &QPushButton::clicked, [=]() {
        lblStatus->setText(QObject::tr("Scanne nach Netzwerken..."));
        lblStatus->setStyleSheet("color: orange;");
        manager->scanNetworks();
    });

    QObject::connect(manager, &NetworkManager::scanFinished, [=](const QStringList &ssids) {
        ssidList->clear();
        ssidList->addItems(ssids);
        if (ssids.isEmpty()) {
            lblStatus->setText(QObject::tr("Keine Netzwerke gefunden"));
            lblStatus->setStyleSheet("color: red;");
        } else {
            lblStatus->setText(QObject::tr("%1 Netzwerke gefunden").arg(ssids.size()));
            lblStatus->setStyleSheet("color: green;");
        }
    });

    QObject::connect(btnConnect, &QPushButton::clicked, [=]() {
        QListWidgetItem *item = ssidList->currentItem();
        if (!item) {
            QMessageBox::warning(networkMenu, QObject::tr("Hinweis"), QObject::tr("Bitte ein WLAN auswählen."));
            return;
        }
        QString ssid = item->text();

        bool ok;
        QString password = QInputDialog::getText(networkMenu,
                                                 QObject::tr("Passwort eingeben"),
                                                 QObject::tr("Passwort für %1:").arg(ssid),
                                                 QLineEdit::Password, "", &ok);
        if (!ok)
            return;

        lblStatus->setText(QObject::tr("Verbinde mit %1...").arg(ssid));
        lblStatus->setStyleSheet("color: orange;");
        manager->connectToNetwork(ssid, password);
    });

    QObject::connect(manager, &NetworkManager::connectionStatus, [=](const QString &msg, bool success) {
        lblStatus->setText(msg);
        lblStatus->setStyleSheet(success ? "color: green;" : "color: red;");
    });

    QObject::connect(manager, &NetworkManager::logMessage, [=](const QString &msg) {
        qDebug() << "[NetworkManager]" << msg;
    });

    QObject::connect(btnBack, &QPushButton::clicked, [=]() {
        stack->setCurrentWidget(systemMenu);
    });

    networkMenu->setLayout(layout);
    return networkMenu;
}
