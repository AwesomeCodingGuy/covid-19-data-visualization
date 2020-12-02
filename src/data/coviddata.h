#ifndef COVIDDATA_H
#define COVIDDATA_H

#include "coviddatatreeitem.h"

class CovidData
{
public:
    CovidData();
    ~CovidData();

    void addDataSet(CovidDataTreeItem &item);

    const CovidDataTreeItem& getDataSets() const;

private:
    int datasetId;
    CovidDataTreeItem dataSets;
};

#endif // COVIDDATA_H
