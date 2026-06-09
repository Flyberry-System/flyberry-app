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
#include "StartupDialog.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>




void initLogging()
{
    auto logger = spdlog::basic_logger_mt("app", "app.log");
    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%H:%M:%S] [%l] %v");
    spdlog::set_level(spdlog::level::debug);
}


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
    //init log file
    initLogging();
    spdlog::info("FlyberryApp gestartet");
    // --- Fenster ---
    QWidget window;
    window.setWindowTitle(QObject::tr("Flyberry Menü"));
    window.setStyleSheet("background-color: #000000;");
   // window.showFullScreen();

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
    for(QPushButton *b : systemMenu->findChildren<QPushButton*>()) 
    {
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


    StartupDialog *startup = new StartupDialog(&window);
    startup->setFixedSize(300, 150);

    QPoint center = window.geometry().center();
    startup->move(center - QPoint(startup->width()/2, startup->height()/2));

    startup->show();
    startup->raise();
    startup->setFocus();

    return app.exec();
}
