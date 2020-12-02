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
    addVectors(sourceAndResult.casesCumulated, source2.casesCumulated);
    addVectors(sourceAndResult.cases, source2.cases);
    addVectors(sourceAndResult.casesSevenDayAverage, source2.casesSevenDayAverage);
    addVectors(sourceAndResult.deaths, source2.deaths);
    addVectors(sourceAndResult.deathsCumulated, source2.deathsCumulated);
}
