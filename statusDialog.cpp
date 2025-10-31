#include "statusDialog.h"

StatusDialog::StatusDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(this);

    textArea = new QTextEdit(this);
    textArea->setReadOnly(true);
    textArea->setStyleSheet("background-color: #222; color: white; font-size: 16px; border-radius: 8px;");
    layout->addWidget(textArea);

    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    layout->addWidget(progressBar);

    setLayout(layout);
    resize(400, 250);
}

void StatusDialog::appendStatus(const QString &text)
{
    textArea->append(text);
    QApplication::processEvents();
}

void StatusDialog::showProgressBar(bool visible)
{
    progressBar->setVisible(visible);
}

void StatusDialog::setProgress(int percent)
{
    progressBar->setValue(percent);
}

void StatusDialog::centerOnParent()
{
	if (parentWidget()) {
		QRect parentRect = parentWidget()->geometry();
		int x = parentRect.x() + (parentRect.width() - width()) / 2;
		int y = parentRect.y() + (parentRect.height() - height()) / 2;
		move(x, y);
	} else {
		// Qt6: zentriere auf primären Bildschirm
		QScreen *screen = QGuiApplication::primaryScreen();
		if (screen) {
			QRect screenRect = screen->geometry();
			int x = (screenRect.width() - width()) / 2;
			int y = (screenRect.height() - height()) / 2;
			move(x, y);
		}
	}
}
