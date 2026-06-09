#include "StartupDialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QKeyEvent>
#include <QProcess>

StartupDialog::StartupDialog(QWidget *parent)
    : QWidget(parent), seconds(3)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setStyleSheet(
        R"(
        background-color: black;
        color: white;
        font-size: 14px;

        border: 3px solid white;
        border-radius: 5px;
        padding: 0px;
        )"
    );

    QVBoxLayout *layout = new QVBoxLayout(this);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);

    label->setWordWrap(true);

    layout->addWidget(label);

    updateText();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &StartupDialog::tick);
    timer->start(1000);
}

void StartupDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) {
        startApp();
    }
    else if (event->key() == Qt::Key_Escape) {
        timer->stop();
        close();
    }
    else {
        QWidget::keyPressEvent(event);
    }
}

void StartupDialog::updateText()
{
    label->setText(
        QString("XCSoar startet in %1...\n\n[ENTER = sofort | ESC = abbrechen]")
        .arg(seconds)
    );
}

void StartupDialog::tick()
{
    seconds--;
    updateText();

    if (seconds <= 0) {
        startApp();
    }
}

void StartupDialog::startApp()
{
    timer->stop();

    QProcess::startDetached("xcsoar");

    close();
}