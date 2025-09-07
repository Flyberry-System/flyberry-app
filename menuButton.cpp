#include "menuButton.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QStyle>

QPushButton* createMenuButton(const QString &text, bool iconRight, QWidget *parent)
{
    QPushButton *btn = new QPushButton(parent);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    btn->setMinimumHeight(60);

    btn->setStyleSheet(R"(
        QPushButton {
            font-size: 20px;
            text-align: center;
            background-color: #2E3440;
            color: #D8DEE9;
            border: none;
            padding: 10px;
        }
        QPushButton:hover {
            background-color: #4C566A;
        }
        QPushButton:pressed {
            background-color: #434C5E;
        }
    )");

    if (iconRight) {
        QHBoxLayout *hLayout = new QHBoxLayout(btn);
        hLayout->setContentsMargins(12,0,12,0);
        hLayout->setSpacing(0);

        hLayout->addStretch();

        QLabel *lblText = new QLabel(text, btn);
        lblText->setAlignment(Qt::AlignCenter);
        lblText->setStyleSheet("color: #D8DEE9; font-size: 20px;");
        hLayout->addWidget(lblText, 0, Qt::AlignCenter);

        hLayout->addStretch();

        QLabel *lblIcon = new QLabel(btn);
        QIcon arrowIcon = QApplication::style()->standardIcon(QStyle::SP_ArrowForward);
        lblIcon->setPixmap(arrowIcon.pixmap(20,20));
        hLayout->addWidget(lblIcon, 0, Qt::AlignRight);

        btn->setLayout(hLayout);
    } else {
        btn->setText(text);
    }

    return btn;
}
