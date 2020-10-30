#ifndef GERMANY_H
#define GERMANY_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QDateTime>

#include "countrycode.h"
#include "casedata.h"

enum class DE_Code {
    DE_SH = 1,
    DE_HH = 2,
    DE_NI = 3,
    DE_HB = 4,
    DE_NW = 5,
    DE_HE = 6,
    DE_RP = 7,
    DE_BW = 8,
    DE_BY = 9,
    DE_SL = 10,
    DE_BE = 11,
    DE_BB = 12,
    DE_MV = 13,
    DE_SN = 14,
    DE_ST = 15,
    DE_TH = 16,
};

struct District
{
    int ags;
    QString name;
    CaseData data;
};

struct State
{
    DE_Code id;
    QString name;
    QString code;
    CaseData data;

    QMap<int, District> districts;
};

struct Country
{
    CountryCode countryCode;
    QString name;
    QString code;
    CaseData data;

    QVector<State> states;
    QVector<QDateTime> timestamps;
    QMap<int, District*> districtMap;
};

class Germany
{
public:
    Germany();
    ~Germany();

    State* getStateByName(const QString &state);
    State* getStateByCode(const QString &code);
    District* getDistrictByAgs(const QString &ags);
    District* getDistrictByAgs(int ags);

    bool loadData(const QString &folder);

private:
    void init();
    void initStates();
    void initDistricts();

    Country data;
};

#endif // GERMANY_H
