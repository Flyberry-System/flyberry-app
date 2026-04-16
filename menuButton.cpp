#include "menuButton.h"
#include <QPushButton>
#include <QKeyEvent>

class MenuButton : public QPushButton
{
public:
    using QPushButton::QPushButton;

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Return ||
            event->key() == Qt::Key_Enter ||
            event->key() == Qt::Key_Space)
        {
            click();
            return;
        }

        QPushButton::keyPressEvent(event);
    }
};

QPushButton* createMenuButton(const QString &text, bool iconRight, QWidget *parent)
{
    MenuButton *btn = new MenuButton(parent);

    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    btn->setMinimumHeight(60);

    btn->setFocusPolicy(Qt::StrongFocus);   // WICHTIG für Keyboard-Navigation

    btn->setStyleSheet(R"(
        QPushButton {
            font-size: 22px;
            text-align: center;
            background-color: #ffffff;
            color: #000000;
            border: 1px solid #999999;
            padding: 10px;
            border-radius: 0px;
        }
        QPushButton:hover {
            background-color: #929292;
        }
        QPushButton:pressed {
            background-color: #414141;
        }
    )");

    btn->setText(text);

    return btn;
}