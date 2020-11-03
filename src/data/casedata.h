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

    void clear();
    void fill(int value, int count);
};

void addCaseData(CaseData &sourceAndResult, const CaseData &source2);

#endif // CASEDATA_H
