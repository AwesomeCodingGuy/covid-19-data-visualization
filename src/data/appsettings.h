#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QObject>

class AppSettings : public QObject
{
    Q_OBJECT
public:
    AppSettings(QObject *parent = nullptr);

public:
    QString downloadFolder;

    QString getLanguageCode() const;
    void setLanguageCode(const QString &value);

signals:
    void languageChanged();

private:
    QString languageCode;
};

#endif // APPSETTINGS_H
