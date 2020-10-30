#include "coviddata.h"

CovidData::CovidData()
{

}

CovidData::~CovidData()
{

}

bool CovidData::loadGermanData(const QString &folder)
{
    return germany.loadData(folder);
}
