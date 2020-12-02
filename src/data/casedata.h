#ifndef CASEDATA_H
#define CASEDATA_H

#include <QDate>
#include <QVector>

struct CaseData
{
    QVector<int> casesCumulated;
    QVector<int> cases;
    QVector<float> casesSevenDayAverage;
    QVector<int> deathsCumulated;
    QVector<int> deaths;
    QDate startDate;
    int population;

    void clear();
    void clearAll();
    void fill(int value, int count);
};

void addCaseData(CaseData &sourceAndResult, const CaseData &source2);

#endif // CASEDATA_H
