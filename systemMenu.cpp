#include "systemMenu.h"
#include "menuButton.h"
#include "networkMenu.h"
#include "settingsMenu.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QObject>

static QPushButton *btnNetwork;

QWidget* createSystemMenu(QStackedWidget *stack, QWidget *mainMenu)
{
    QWidget *menu = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(menu);
    layout->setSpacing(5);
    layout->setContentsMargins(5,5,5,5);

    // Netzwerk-Button
    QPushButton *btnUpdateSystem = createMenuButton(QObject::tr("Update System"));
	layout->addWidget(btnUpdateSystem);

    // Netzwerk-Button
    btnNetwork = createMenuButton(QObject::tr("Netzwerk"));
    layout->addWidget(btnNetwork);

    // Einstellungen-Button
    QPushButton *btnSettings = createMenuButton(QObject::tr("Einstellungen"));
    layout->addWidget(btnSettings);

    // Klick auf Einstellungen → Wechsel ins Settings-Menü
    QObject::connect(btnSettings, &QPushButton::clicked, [=]() {
        QWidget *settingsMenu = createSettingsMenu(stack, menu);
        stack->addWidget(settingsMenu);
        stack->setCurrentWidget(settingsMenu);
    });

    // SystemInfo-Button
    QPushButton *btnSystemInfo = createMenuButton(QObject::tr("System Info"));
    layout->addWidget(btnSystemInfo);

    // Zurück-Button
    QPushButton *btnBack = createMenuButton(QObject::tr("Zurück"));
    QObject::connect(btnBack, &QPushButton::clicked, [=]() {
        stack->setCurrentWidget(mainMenu);
    });
    layout->addWidget(btnBack);

    layout->addStretch();

    // Klick auf Netzwerk → Wechsel zu Network-Menü
    QObject::connect(btnNetwork, &QPushButton::clicked, [=]() {
        QWidget *networkMenu = createNetworkMenu(stack, menu);
        stack->addWidget(networkMenu);
        stack->setCurrentWidget(networkMenu);
    });

    menu->setLayout(layout);
    return menu;
}
