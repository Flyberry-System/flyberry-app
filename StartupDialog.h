#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QWidget>

class QLabel;
class QTimer;
class QKeyEvent;

class StartupDialog : public QWidget
{
    Q_OBJECT

public:
    explicit StartupDialog(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QLabel *label;
    QTimer *timer;
    int seconds;

    void updateText();
    void tick();
    void startApp();
};

#endif // STARTUPDIALOG_H