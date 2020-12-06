#ifndef CASEDATA_H
#define CASEDATA_H

#include <QDate>
#include <QVector>

#include <algorithm>

#include "../utils/utility.h"

template<typename T>
struct CaseSeries
{
    T min{0};
    T max{0};
    QVector<T> series;

    void clear() {
        series.clear();
    }

    void fill(int value, int count) {
        series.fill(value, count);
    }

    CaseSeries& operator=(const QVector<T> &data) {
        series = data;
        calculateMinMax(series, min, max);
        return *this;
    }
};

struct CaseData
{
    CaseSeries<int> casesCumulated;
    CaseSeries<int> cases;
    CaseSeries<float> casesSevenDayAverage;
    CaseSeries<int> deathsCumulated;
    CaseSeries<int> deaths;
    QDate startDate;
    int population;

    void clear();
    void clearAll();
    void fill(int value, int count);
};

template<typename T>
void calculateMinMax(CaseSeries<T> &s) {
    calculateMinMax(s.series, s.min, s.max);
}

void addCaseData(CaseData &sourceAndResult, const CaseData &source2);

#endif // CASEDATA_H
