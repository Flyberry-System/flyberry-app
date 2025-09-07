#ifndef NETWORKMENU_H
#define NETWORKMENU_H

#include <QWidget>
#include <QStackedWidget>

// Erstellt das Network-Menü-Widget
// stack: Zeigt auf das QStackedWidget, um zwischen Menüs zu wechseln
// systemMenu: Zeigt auf das System-Menü, um bei "Zurück" zurück zu wechseln
QWidget* createNetworkMenu(QStackedWidget *stack, QWidget *systemMenu);

#endif // NETWORKMENU_H

