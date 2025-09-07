#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <QWidget>
#include <QStackedWidget>
#include <QTranslator>
#include <QCoreApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QDialog>
#include <QLabel>
#include <QSlider>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QComboBox>
#include <QRegularExpression>
#include "menuButton.h"
#include <QKeyEvent>
#include <QProcess>
#include "EnterEventFilter.h"
#include "languageHelper.h"

QWidget* createSettingsMenu(QStackedWidget *stack, QWidget *systemMenu);

#endif // SETTINGSMENU_H
