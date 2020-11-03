#ifndef WORLD_H
#define WORLD_H

#include <QString>
#include <QDateTime>

#include "casedata.h"

namespace world {

struct Location {
    float latitude;
    float longitude;
};

struct Province
{
    QString name;
    Location location;
    CaseData data;
};

struct Country {
    QString name;
    Location location;
    CaseData data;

    QVector<Province> provinces;
};

class World
{
    enum class FileType {
        Cases,
        Deaths
    };

public:
    World();
    ~World();

    Province* getProvinceByName(const QString &name);
    Province* getProvinceByName(Country &country, QString &name);
    Country* getCountryByName(const QString &name);
    bool getCaseDataByName(const QString &country, const QString &province,
                           CaseData &caseData, QVector<QDateTime> &timestamps);


    bool loadData(const QString &folder);

private:
    bool readCsvByCountry(const QString &filename, FileType fileType);
    void calculateTotalValues();

public:
    QString name;
    CaseData data;

    QVector<QDateTime> timestamps;
    QVector<Country> countries;
};

} // namespace world

#endif // WORLD_H
