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
    addVectors(sourceAndResult.cases.series, source2.cases.series);
    addVectors(sourceAndResult.casesSevenDayAverage.series, source2.casesSevenDayAverage.series);
    addVectors(sourceAndResult.deaths.series, source2.deaths.series);
    addVectors(sourceAndResult.deathsCumulated.series, source2.deathsCumulated.series);
}
