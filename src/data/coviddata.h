#ifndef COVIDDATA_H
#define COVIDDATA_H

#include "germany.h"

class CovidData
{
public:
    CovidData();
    ~CovidData();

    bool loadGermanData(const QString &folder);

private:
    Germany germany;
};

#endif // COVIDDATA_H
