#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QProcess>
#include <QTimer>
#include "networkMenu.h"
#include "menuButton.h"
#include <QTime>
#include "systemMenu.h"
#include "statusBar.h"
#include "mainMenu.h"
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    // --- Übersetzer ---
//	QTranslator translator;
//	QString locale = QLocale::system().name(); // z.B. "de_DE"
//	QString qmFile = QString("%1/flyberry_en.qm").arg(QString(QT_TRANSLATIONS_DIR));
//
//	if(translator.load(qmFile)) {
//		app.installTranslator(&translator);
//	}

    // --- Fenster ---
    QWidget window;
    window.setWindowTitle(QObject::tr("Flyberry Menü"));
    window.setStyleSheet("background-color: #3B4252;");

    QVBoxLayout *windowLayout = new QVBoxLayout(&window);
    windowLayout->setContentsMargins(0,0,0,0);

    // Statusbar
    StatusBar *statusBar = new StatusBar(&window);
    windowLayout->addWidget(statusBar);

    // Stack für Menüs
    QStackedWidget *stack = new QStackedWidget(&window);
    QWidget *systemMenu = createSystemMenu(stack, nullptr);
    MainMenu *mainMenu = new MainMenu(stack, systemMenu);

    // "Zurück"-Button verbinden
    for(QPushButton *b : systemMenu->findChildren<QPushButton*>()) {
        if(b->text() == QObject::tr("Zurück")) {
            QObject::connect(b, &QPushButton::clicked, [=]() {
                stack->setCurrentWidget(mainMenu);
            });
            break;
        }
    }

    stack->addWidget(mainMenu);
    stack->addWidget(systemMenu);

    windowLayout->addWidget(stack);

    window.resize(480,800);
    window.show();

    return app.exec();
}
