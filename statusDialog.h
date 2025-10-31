#pragma once
#include <QDialog>
#include <QTextEdit>
#include <QProgressBar>
#include <QScreen>
#include <QVBoxLayout>
#include <QTimer>
#include <QApplication>

class StatusDialog : public QDialog {
    Q_OBJECT
public:
    explicit StatusDialog(QWidget *parent = nullptr);

    void appendStatus(const QString &text);
    void appendMessage(const QString &text) { appendStatus(text); }  // Alias
    void showProgressBar(bool visible);
    void setProgress(int percent);
    void centerOnParent();  // optionales Zentrieren des Dialogs

private:
    QTextEdit *textArea;
    QProgressBar *progressBar;
};
