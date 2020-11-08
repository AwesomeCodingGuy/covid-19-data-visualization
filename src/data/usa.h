#ifndef USA_H
#define USA_H

#include <QString>
#include <QVector>
#include <QDateTime>

#include "casedata.h"

namespace usa {

struct Uid
{
    int uid;
    int code3;
    int stateId;
    int fips;

    Uid();
    Uid(int uid);
    Uid(int code3, int stateId, int fips);
    Uid(int uid, int code3, int stateId, int fips);
};

struct Location
{
    double latitude;
    double longitude;
};

struct County
{
    Uid uid;
    QString name;
    Location location;
    CaseData data;
    int population;
};

struct State
{
    QString name;
    QString code;
    Location location;
    Uid uid;
    CaseData data;
    int population;

    QVector<County> counties;
};

class Usa
{
    enum class FileType {
        Cases,
        Deaths
    };

public:
    Usa();
    ~Usa();

    County* getCountyByName(const QString &name);
    County* getCountyByName(State &state, const QString &name);
    State* getStateByName(const QString &name);

    QString getNameOfCounty(const Uid uid) const;
    QString getNameOfState(const Uid uid) const;

    const QVector<QDateTime> getTimestamps() const;
    const CaseData* getCaseDataByUid(const Uid &uid) const;

    bool loadData(const QString &folder);

private:
    void initStates();

    bool readCsvByCounty(const QString &filename, FileType fileType);

    void calculateSummedValues();

public:
    QString name;
    QString code;
    Uid uid;
    CaseData data;
    int population;

    QVector<State> states;

    QVector<QDateTime> timestamps;
};

} // namespace usa

#endif // USA_H
