/*
 * updateMenu.h
 *
 *  Created on: 28.10.2025
 *      Author: markus
 */

#ifndef UPDATEMENU_H_
#define UPDATEMENU_H_

#include <QWidget>
#include <QStackedWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include "menuButton.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include "statusDialog.h"
#include "updateManager.h"
#include <QTimer>

class UpdateMenu : public QWidget {
    Q_OBJECT

public:
    explicit UpdateMenu(QStackedWidget *stack, QWidget *systemMenu, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void refreshStatus();
    void checkForUpdate();
    void startUpdate();

private:
    void parseRaucStatus(const QString &output);

    QLabel *lblActiveSlot;
    QLabel *lblVersion;
    QLabel *lblStatus;
    QTextEdit *logArea;
    QPushButton *btnStartUpdate;
    QPushButton *btnCheck;
    QStackedWidget *stack;
    QWidget *systemMenu;
    QVector<QPushButton*> buttons;
    UpdateManager *updatemanager;
};

#endif /* UPDATEMENU_H_ */
