#ifndef CASEDATA_H
#define CASEDATA_H

#include <QVector>

struct CaseData
{
    QVector<int> casesCumulated;
    QVector<int> cases;
    QVector<float> casesSevenDayAverage;
    QVector<int> deathsCumulated;
    QVector<int> deaths;
};

#endif // CASEDATA_H
