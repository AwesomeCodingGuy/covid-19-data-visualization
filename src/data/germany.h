#ifndef GERMANY_H
#define GERMANY_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QDateTime>

#include "casedata.h"

namespace germany {

enum class DE_Code {
    DE    = 0,
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
    DE_Code id;
    QString name;
    QString code;
    CaseData data;

    QVector<State> states;
    QVector<QDateTime> timestamps;
    QMap<int, District*> districtMap;
};

class Germany
{
    enum class FileType {
        Cases,
        Deaths
    };

public:
    Germany();
    ~Germany();

    QString getName(int ags);
    State* getStateByName(const QString &state);
    State* getStateByCode(const QString &code);
    District* getDistrictByAgs(const QString &ags);
    District* getDistrictByAgs(int ags);
    const Country& getCountryData() const;
    bool getCaseDataByCode(QString code,
                           CaseData &caseData,
                           QVector<QDateTime> &timestamps);
    const CaseData* getCaseDataByAgs(const int ags);
    const QVector<QDateTime> getTimestamps() const;

    bool loadData(const QString &folder);

private:
    void init();
    void initStates();

    bool readJsonAgs(QString filename);
    bool readCsvByState(QString filename, FileType fileType);
    bool readCsvByAgs(QString filename, FileType fileType);

    Country country;
};

} // namespace germany

#endif // GERMANY_H
