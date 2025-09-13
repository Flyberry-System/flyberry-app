// MainMenu.cpp
#include "mainMenu.h"


MainMenu::MainMenu(QStackedWidget* stack_, QWidget* systemMenu_, QWidget* parent)
    : QWidget(parent), stack(stack_), systemMenu(systemMenu_)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);
    layout->setContentsMargins(5,5,5,5);

    btnXCSoar   = createMenuButton(tr("Starte XCSoar"));
    btnSystem   = createMenuButton(tr("System"), true);
    btnShell    = createMenuButton(tr("Shell"));
    btnReboot   = createMenuButton(tr("Neustart"));
    btnShutdown = createMenuButton(tr("Ausschalten"));

    layout->addWidget(btnXCSoar);
    layout->addWidget(btnSystem);
    layout->addWidget(btnShell);
    layout->addWidget(btnReboot);
    layout->addWidget(btnShutdown);
    layout->addStretch();

    // Aktionen
    QObject::connect(btnXCSoar, &QPushButton::clicked, []() {
        QProcess::startDetached("xcsoar", QStringList() << "-fly");
    });

    QObject::connect(btnSystem, &QPushButton::clicked, [=]() {
        stack->setCurrentWidget(systemMenu);
    });

    QObject::connect(btnShell, &QPushButton::clicked, [=]() {
        qApp->quit();
    });

    QObject::connect(btnReboot, &QPushButton::clicked, []() {
        QProcess::startDetached("reboot");
    });

    QObject::connect(btnShutdown, &QPushButton::clicked, []() {
        QProcess::startDetached("poweroff");
    });

    // Language update
    connect(LanguageHelper::instance(), &LanguageHelper::languageChanged, this, &MainMenu::retranslateUi);
}

void MainMenu::retranslateUi() {
    btnXCSoar->setText(tr("Starte XCSoar"));
    btnSystem->setText(tr("System"));
    btnShell->setText(tr("Shell"));
    btnReboot->setText(tr("Neustart"));
    btnShutdown->setText(tr("Ausschalten"));
}
