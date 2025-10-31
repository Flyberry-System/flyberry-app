#include "confirmDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QFrame>

ConfirmDialog::ConfirmDialog(const QString &message, QWidget *parent)
    : QDialog(parent)
{
    // Fenster ohne Titel & Rahmen
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setModal(true);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);

    // --- Äußerer transparenter Layout-Container ---
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setAlignment(Qt::AlignCenter);

    // --- Innenrahmen mit Hintergrund und Rundung ---
    QFrame *frame = new QFrame(this);
    frame->setObjectName("confirmFrame");
    frame->setStyleSheet(
        "#confirmFrame { "
        "background-color: rgba(30, 30, 30, 240); "
        "border-radius: 20px; "
        "border: 2px solid #555;"
        "}"
    );
    frame->setFixedSize(500, 200);

    // --- Inhalt im Frame ---
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    label = new QLabel(message, frame);
    label->setAlignment(Qt::AlignCenter);
    QFont f = label->font();
    f.setPointSize(14);
    label->setFont(f);
    label->setStyleSheet("color: white;");
    layout->addWidget(label);

    // --- Button-Reihe ---
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnYes = new QPushButton(tr("Ja"), frame);
    btnNo  = new QPushButton(tr("Nein"), frame);

    btnYes->setMinimumWidth(100);
    btnNo->setMinimumWidth(100);

    QString baseStyle =
        "QPushButton { color: white; font-weight: bold; border-radius: 8px; padding: 8px; }"
        "QPushButton:focus { border: 2px solid white; }";

    btnYes->setStyleSheet(baseStyle + "QPushButton { background-color: #2ecc71; }");
    btnNo->setStyleSheet(baseStyle + "QPushButton { background-color: #e74c3c; }");

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnYes);
    buttonLayout->addWidget(btnNo);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);

    outerLayout->addWidget(frame, 0, Qt::AlignCenter);

    // Fokus & Aktionen
    btnYes->setFocus();
    connect(btnYes, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnNo,  &QPushButton::clicked, this, &QDialog::reject);

    // --- Fenster zentrieren ---
	if (parentWidget()) {
		QRect parentRect = parentWidget()->geometry();
		move(parentRect.center() - rect().center());
	} else {
		QScreen *screen = QGuiApplication::primaryScreen();
		QRect screenRect = screen->availableGeometry();
		move(screenRect.center() - rect().center());
	}
}

void ConfirmDialog::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Right:
            if (btnYes->hasFocus())
                btnNo->setFocus();
            else
                btnYes->setFocus();
            break;

        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (btnYes->hasFocus())
                accept();
            else
                reject();
            break;

        case Qt::Key_Escape:
            reject();
            break;

        default:
            QDialog::keyPressEvent(event);
    }
}
