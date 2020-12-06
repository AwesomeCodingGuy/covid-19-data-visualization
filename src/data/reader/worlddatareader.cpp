#include "worlddatareader.h"

#include "../constants.h"
#include "../../utils/utility.h"

#include <QFile>

constexpr int firstDataIndex = 4;

WorldDataReader::WorldDataReader(const QString &folder, CovidDataTreeItem &dataItem)
    : rootItem(dataItem)
{
    confirmedGlobalFile = QString(folder).append(constants::casesGlobal);
    deathsGlobalFile    = QString(folder).append(constants::deathsGlobal);
}

WorldDataReader::~WorldDataReader()
{

}

bool WorldDataReader::validateFiles() const
{
    return (QFile::exists(confirmedGlobalFile)
         && QFile::exists(deathsGlobalFile));
}

bool WorldDataReader::loadData()
{
    if(!validateFiles()) {
        return false;
    }

    // set new item to clear existing data.
    rootItem = CovidDataTreeItem("World");

    // get the start date of the data
    startDate = getStartDateOfDataSet();

    // first read case and after the deaths file
    if(!readCsvFileByCounty(confirmedGlobalFile, FileType::Cases)) {
        return false;
    }

    if(!readCsvFileByCounty(deathsGlobalFile, FileType::Deaths)) {
        return false;
    }

    // calculate total values for the world, and for countries with provinces
    calculateTotalvalues();

    return true;
}

bool WorldDataReader::readCsvFileByCounty(const QString &filename, FileType fileType)
{
    // open file
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // process first line (headline)
    QList<QByteArray> captions = file.readLine().split(',');
    dataValues = captions.size() - firstDataIndex;

    // iterate over lines in file and add counties and data
    while(!file.atEnd()) {
        // get line of comma separated line
        QByteArray line = file.readLine();
        escapeCommasInStrings(line);
        auto entries = line.split(',');
        resolveEscapes(entries);

        // get data
        QString nameOfProvince  = entries[0];
        QString nameOfCountry   = entries[1];
        QVector<int> data(dataValues);
        for(int i = firstDataIndex; i < entries.size(); ++i) {
            data[i - firstDataIndex] = entries[i].toInt();
        }

        // get country
        CovidDataTreeItem *country = rootItem.getChild(nameOfCountry);

        // if country is not in root item => add it. Index is incremented here
        if(!country) {
            CovidDataTreeItem newStateItem(nameOfCountry);
            rootItem.addChild(rootItem.getChildren().size(), newStateItem);

            // get previously added item
            country = rootItem.getChild(nameOfCountry);
        }

        // if province name is empty => add data to its country, else add the province to the country
        CovidDataTreeItem *dataItemPtr;
        if(nameOfProvince.isEmpty()) {
            dataItemPtr = country;
        } else {
            CovidDataTreeItem *province = country->getChild(nameOfProvince);

            // add province if it is not already in data
            if(!province) {
                CovidDataTreeItem newProvinceItem(nameOfProvince);
                country->addChild(country->getChildren().size(), newProvinceItem);

                province = country->getChild(nameOfProvince);
            }

            dataItemPtr = province;
        }

        // add data
        if(fileType == FileType::Cases) {
            dataItemPtr->getCaseData().casesCumulated       = data;
            dataItemPtr->getCaseData().cases                = calculateIncrease(data);
            dataItemPtr->getCaseData().casesSevenDayAverage = calculateAveragedIncrease(dataItemPtr->getCaseData().cases.series);
        } else if(fileType == FileType::Deaths) {
            dataItemPtr->getCaseData().deathsCumulated      = data;
            dataItemPtr->getCaseData().deaths               = calculateIncrease(data);
            dataItemPtr->getCaseData().population           = 0;
            dataItemPtr->getCaseData().startDate            = startDate;
        }
    }

    return true;
}

void WorldDataReader::calculateTotalvalues()
{
    // if the rootitem has children, they can be added
    if(rootItem.getChildren().size() > 0) {
        CaseData &rootItemCases = rootItem.getCaseData();

        // init the case vectors with the data size of the first child
        rootItemCases.fill(0, dataValues);
        rootItemCases.population = 0;
        rootItemCases.startDate  = startDate;

        // iterate over all countries
        for(CovidDataTreeItem &countryItem : rootItem.getChildren()) {
            CaseData &countryItemCases = countryItem.getCaseData();

            // before adding country data, add its province data to the country
            if(countryItem.getChildren().size() > 0) {
                // if there is data for the heartland, move the data to a province first
                if(countryItemCases.casesCumulated.series.size() > 0) {
                    // append "Heartland" to the item, to clearify that this is not really a province
                    CovidDataTreeItem newProvince(QString(countryItem.getItemNameAlt().append(" (Heartland)")));
                    newProvince.setCaseData(countryItem.getCaseData());

                    countryItem.addChild(countryItem.getChildren().size(), newProvince);
                }

                // now that the data is moved down to the provinces, its datra can be cleared and summed up
                countryItemCases.fill(0, dataValues);
                countryItemCases.population = 0;
                countryItemCases.startDate  = startDate;

                // add data of provinces to country data
                for(CovidDataTreeItem &provinceItem : countryItem.getChildren()) {
                    countryItemCases.population += provinceItem.getCaseData().population;
                    addCaseData(countryItemCases, provinceItem.getCaseData());
                }
            }

            // add country data
            rootItemCases.population += countryItemCases.population;
            addCaseData(rootItemCases, countryItemCases);
        }
    }
}

QDate WorldDataReader::getStartDateOfDataSet()
{
    //open file
    QFile file;
    file.setFileName(confirmedGlobalFile);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // get first entry of second line in file
    auto dateParts = file.readLine().split(',')[firstDataIndex].split('/');
    QDate date(dateParts[2].toInt() + 2000,
               dateParts[0].toInt(),
               dateParts[1].toInt());

    // close file
    file.close();

    // return date
    return date;
}
