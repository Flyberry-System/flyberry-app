#pragma once
#include <QDialog>
#include <QShowEvent>
#include <QDebug>

class QPushButton;
class QLabel;

class ConfirmDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConfirmDialog(const QString &message, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override; 

private:
    QLabel *label;
    QPushButton *btnYes;
    QPushButton *btnNo;
};
