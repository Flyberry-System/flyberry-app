/*
 * EnterEventFilter.h
 *
 *  Created on: 30.08.2025
 *      Author: markus
 */

#ifndef ENTEREVENTFILTER_H_
#define ENTEREVENTFILTER_H_

#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <functional>

class EnterEventFilter : public QObject {
public:
    explicit EnterEventFilter(std::function<void()> cb, QObject *parent = nullptr);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    std::function<void()> callback;
};


#endif /* ENTEREVENTFILTER_H_ */
