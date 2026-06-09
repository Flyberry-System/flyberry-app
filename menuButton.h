#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include <QPushButton>
#include <QString>
#include <QWidget>

// Erstellt einen QPushButton mit optionalem Pfeil rechts
QPushButton* createMenuButton(const QString &text, bool iconRight=false, QWidget *parent=nullptr);

#endif // MENUBUTTON_H
