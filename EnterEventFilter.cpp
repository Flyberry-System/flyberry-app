#include "EnterEventFilter.h"


EnterEventFilter::EnterEventFilter(std::function<void()> cb, QObject *parent)
    : QObject(parent), callback(cb) {}

bool EnterEventFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            callback();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
