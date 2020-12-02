#include "coviddata.h"

#include "coviddatatreeitem.h"

CovidData::CovidData()
    : datasetId(0)
{
    dataSets.setItemNameAlt("root");
}

CovidData::~CovidData()
{

}

void CovidData::addDataSet(CovidDataTreeItem &item)
{
    dataSets.addChild(datasetId++, item);
}

const CovidDataTreeItem& CovidData::getDataSets() const
{
    return dataSets;
}
