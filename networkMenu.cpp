#include "networkMenu.h"
#include "menuButton.h"
#include "networkManager.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QLayout>
#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>

// ---------------- PasswordLineEdit ----------------
class PasswordLineEdit : public QLineEdit 
{
public:
    PasswordLineEdit(QWidget *parent = nullptr, QVBoxLayout *scanLayout = nullptr)
        : QLineEdit(parent), m_scanLayout(scanLayout) {}

    QVBoxLayout *m_scanLayout;

protected:
    QWidget* findFocusableWidget(int startIndex, int direction) {
        int i = startIndex;

        while (true) {
            i += direction;
            if (i < 0 || i >= m_scanLayout->count())
                break;

            QLayoutItem *item = m_scanLayout->itemAt(i);
            QWidget *w = item ? item->widget() : nullptr;

            if (qobject_cast<QPushButton*>(w)) {
                return w;
            }
        }
        return nullptr;
    }

    void keyPressEvent(QKeyEvent *event) override {
        QWidget *container = this->parentWidget(); // inputContainer
        if (!container || !m_scanLayout) {
            QLineEdit::keyPressEvent(event);
            return;
        }

        int index = m_scanLayout->indexOf(container);
        if (index < 0) {
            QLineEdit::keyPressEvent(event);
            return;
        }

        QWidget *nextWidget = nullptr;

        if (event->key() == Qt::Key_Down) {
            qWarning() << "key down" << index;
            nextWidget = findFocusableWidget(index, +1);
        }
        else if (event->key() == Qt::Key_Up) {
            qWarning() << "key up" << index;
            nextWidget = findFocusableWidget(index, -1);
        }
        else {
            QLineEdit::keyPressEvent(event);
            return;
        }

        // Fokus setzen
        if (nextWidget) {
            nextWidget->setFocus();
        }

        container->setVisible(false);
    }
};

// ---------------- createScanMenu ----------------
QWidget* createScanMenu(QStackedWidget *stack,
                        QWidget *networkMenu,
                        NetworkManager *manager)
{
    QWidget *scanMenu = new QWidget();
    QVBoxLayout *scanLayout = new QVBoxLayout(scanMenu);

    QLabel *scanTitle = new QLabel(QObject::tr("Verfügbare Netzwerke"));
    scanTitle->setAlignment(Qt::AlignCenter);
    scanLayout->addWidget(scanTitle);

    QPushButton *btnBack = createMenuButton(QObject::tr("Zurück"));

    QObject::connect(manager, &NetworkManager::scanFinished,
                 [=](const QStringList &ssids) {

        for (const QString &ssid : ssids) {

            // 👉 SSID Button
            QPushButton *btn = createMenuButton(ssid);
            scanLayout->addWidget(btn);

            // 👉 InputContainer direkt erstellen (aber versteckt)
            QWidget *inputContainer = new QWidget(scanMenu);
            QHBoxLayout *inputLayout = new QHBoxLayout(inputContainer);
            inputLayout->setContentsMargins(20, 0, 20, 5);

            PasswordLineEdit *passwordEdit =
                new PasswordLineEdit(inputContainer, scanLayout);

            passwordEdit->setEchoMode(QLineEdit::Password);
            passwordEdit->setPlaceholderText("Passwort eingeben");

            QPushButton *connectBtn = new QPushButton("Verbinden");

            inputLayout->addWidget(passwordEdit);
            inputLayout->addWidget(connectBtn);

            // 👉 initial verstecken
            inputContainer->setVisible(false);

            // 👉 direkt unter Button einfügen
            scanLayout->addWidget(inputContainer);

            // 👉 Klick = anzeigen / verstecken
            QObject::connect(btn, &QPushButton::clicked, [=]() 
            {

                bool visible = inputContainer->isVisible();
                inputContainer->setVisible(!visible);

                if (!visible) {
                    passwordEdit->setFocus();
                }
            });

            // 👉 Connect-Logik
            QObject::connect(connectBtn, &QPushButton::clicked,
                            [passwordEdit, manager, ssid]() 
            {
                manager->connectToNetwork(ssid, passwordEdit->text());
            });
        }

        scanLayout->addWidget(btnBack);
        scanLayout->addStretch();

        stack->setCurrentWidget(scanMenu);
    });

    QObject::connect(btnBack, &QPushButton::clicked, [=]() {
        stack->setCurrentWidget(networkMenu);
    });

    return scanMenu;
}

// ---------------- createNetworkMenu ----------------
QWidget* createNetworkMenu(QStackedWidget *stack, QWidget *systemMenu)
{
    QWidget *networkMenu = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(networkMenu);

    QLabel *lblTitle = new QLabel(QObject::tr("Netzwerk"));
    lblTitle->setAlignment(Qt::AlignCenter);

    QPushButton *btnScan = createMenuButton(QObject::tr("Netzwerke scannen"));
    QPushButton *btnBack = createMenuButton(QObject::tr("Zurück"));

    layout->addWidget(lblTitle);
    layout->addWidget(btnScan);
    layout->addWidget(btnBack);
    layout->addStretch();

    auto *manager = new NetworkManager(networkMenu);

    QObject::connect(btnScan, &QPushButton::clicked, [=]() {
        QWidget *scanMenu = createScanMenu(stack, networkMenu, manager);
        stack->addWidget(scanMenu);
        manager->scanNetworks();
    });

    QObject::connect(btnBack, &QPushButton::clicked, [=]() {
        stack->setCurrentWidget(systemMenu);
    });

    return networkMenu;
}