#ifndef LANGUAGEHELPER_H
#define LANGUAGEHELPER_H

#include <QObject>
#include <QTranslator>
#include <QString>

class LanguageHelper : public QObject
{
    Q_OBJECT

public:
    static LanguageHelper* instance();

    void changeLanguage(const QString &langCode);
    void setTranslator(QTranslator* t);
    QTranslator* getTranslator() const;

signals:
    void languageChanged(); // Signal, wenn Sprache gewechselt wurde

private:
    explicit LanguageHelper(QObject *parent = nullptr);
    QTranslator *translator;
};

#endif // LANGUAGEHELPER_H
