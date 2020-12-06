#include "casedata.h"
#include "../utils/utility.h"

void CaseData::clear()
{
    casesCumulated.clear();
    cases.clear();
    casesSevenDayAverage.clear();
    deathsCumulated.clear();
    deaths.clear();
}

void CaseData::clearAll()
{
    casesCumulated.clear();
    cases.clear();
    casesSevenDayAverage.clear();
    deathsCumulated.clear();
    deaths.clear();
    startDate = QDate();
    population = 0;
}

void CaseData::fill(int value, int count)
{
    casesCumulated.fill(value, count);
    cases.fill(value, count);
    casesSevenDayAverage.fill(value, count);
    deathsCumulated.fill(value, count);
    deaths.fill(value, count);
}

void addCaseData(CaseData &sourceAndResult, const CaseData &source2)
{
    addVectors(sourceAndResult.casesCumulated.series, source2.casesCumulated.series);
    calculateMinMax(sourceAndResult.casesCumulated);

    addVectors(sourceAndResult.cases.series, source2.cases.series);
    calculateMinMax(sourceAndResult.cases);

    addVectors(sourceAndResult.casesSevenDayAverage.series, source2.casesSevenDayAverage.series);
    calculateMinMax(sourceAndResult.casesSevenDayAverage);

    addVectors(sourceAndResult.deaths.series, source2.deaths.series);
    calculateMinMax(sourceAndResult.deaths);

    addVectors(sourceAndResult.deathsCumulated.series, source2.deathsCumulated.series);
    calculateMinMax(sourceAndResult.deathsCumulated);
}
