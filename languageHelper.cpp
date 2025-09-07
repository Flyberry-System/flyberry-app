#include "languageHelper.h"
#include <QApplication>

LanguageHelper* LanguageHelper::instance()
{
    static LanguageHelper helper;
    return &helper;
}

LanguageHelper::LanguageHelper(QObject *parent)
    : QObject(parent), translator(nullptr)
{
}

void LanguageHelper::setTranslator(QTranslator* t)
{
    translator = t;
}

QTranslator* LanguageHelper::getTranslator() const
{
    return translator;
}

void LanguageHelper::changeLanguage(const QString &langCode)
{
    if (!translator)
        translator = new QTranslator();

    QString qmFile = QString("%1/%2.qm").arg(QString(QT_TRANSLATIONS_DIR), langCode);

    if(translator->load(qmFile)) {
        qApp->installTranslator(translator);
        emit languageChanged();
    }
}
