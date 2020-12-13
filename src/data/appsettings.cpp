#include "appsettings.h"

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
{

}

QString AppSettings::getLanguageCode() const
{
    return languageCode;
}

void AppSettings::setLanguageCode(const QString &value)
{
    if(languageCode != value) {
        languageCode = value;
        emit languageChanged();
    }
}
