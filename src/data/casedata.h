#ifndef CASEDATA_H
#define CASEDATA_H

#include <QDate>
#include <QVector>

#include <algorithm>

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
        auto minMax = std::minmax_element(series.begin(), series.end());
        min = *minMax.first;
        max = *minMax.second;
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

void addCaseData(CaseData &sourceAndResult, const CaseData &source2);

#endif // CASEDATA_H
