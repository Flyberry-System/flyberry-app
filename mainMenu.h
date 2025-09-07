// MainMenu.h
#pragma once
#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>

class MainMenu : public QWidget {
    Q_OBJECT
public:
    MainMenu(QStackedWidget* stack, QWidget* systemMenu, QWidget* parent=nullptr);

public slots:
    void retranslateUi();

private:
    QPushButton *btnXCSoar, *btnSystem, *btnShell, *btnReboot, *btnShutdown;
    QStackedWidget* stack;
    QWidget* systemMenu;
};
