#include "usadatareader.h"

#include "../constants.h"
#include "../../utils/utility.h"
#include "../../utils/shapelibutils.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

constexpr int firstDataIndex = 12;

UsaDataReader::UsaDataReader(const QString &folder, CovidDataTreeItem &dataItem)
    : rootItem(dataItem)
{
    usaStatesFile       = constants::usaStateJson;
    mapShapeFile        = constants::usaShapeFile;
    confirmedUsaFile    = QString(folder).append(constants::casesUS);
    deathsUsaFile       = QString(folder).append(constants::deathsUS);
}

UsaDataReader::~UsaDataReader()
{

}

bool UsaDataReader::validateFiles() const
{
    return (QFile::exists(usaStatesFile)
         && QFile::exists(mapShapeFile)
         && QFile::exists(confirmedUsaFile)
         && QFile::exists(deathsUsaFile));
}

bool UsaDataReader::loadData()
{
    if(!validateFiles()) {
        return false;
    }

    // set new item to clear existing data.
    rootItem = CovidDataTreeItem();

    // get the start date of the data
    startDate = getStartDateOfDataSet();

    // init States
    initStates();

    // first read case and after the deaths file
    if(!readCsvFileByCounty(confirmedUsaFile, FileType::Cases)) {
        return false;
    }

    if(!readCsvFileByCounty(deathsUsaFile, FileType::Deaths)) {
        return false;
    }

    // read shapefile
    if(!loadMapData()) {
        return false;
    }

    // sums all cases and population of sub items
    // the function assumes that the startDate of all cases is the same.
    calculateCaseDataSums();

    return true;
}

void UsaDataReader::initStates()
{
    // open file
    QFile stateFile;
    stateFile.setFileName(usaStatesFile);
    stateFile.open(QIODevice::ReadOnly | QIODevice::Text);

    // read file content and close file
    QString jsonContent = stateFile.readAll();
    stateFile.close();

    // get json root object
    QJsonDocument doc = QJsonDocument::fromJson(jsonContent.toUtf8());
    QJsonObject jsonRoot = doc.object();

    // init rootItem name
    rootItem.setItemNameAlt(jsonRoot.value("name").toString());

    // get state data
    QJsonArray states = jsonRoot.value("states").toArray();
    for(const QJsonValue &v : states) {
        CovidDataTreeItem newStateItem(v.toObject().value("name").toString());
        rootItem.addChild(v.toObject().value("fips").toInt(), newStateItem);
    }
}

bool UsaDataReader::readCsvFileByCounty(const QString &filename, UsaDataReader::FileType fileType)
{
    // open file
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // process first line (headline)
    QList<QByteArray> captions = file.readLine().split(',');
    const int countyDataValues = (fileType == FileType::Cases ? 11 : 12);
    dataValues = captions.size() - countyDataValues;

    // iterate over lines in file and add counties and data
    while(!file.atEnd()) {
        // get line of comma separated line
        QByteArray line = file.readLine();
        escapeCommasInStrings(line);
        auto entries = line.split(',');
        resolveEscapes(entries);

        // get data
        QString nameOfCounty = entries[5];
        QString nameOfState = entries[6];
        int fips = getFips(entries[0], entries[4]);
        double lat = entries[8].toDouble();
        double lon = entries[9].toDouble();
        QVector<int> data(dataValues);
        for(int i = countyDataValues; i < entries.size(); ++i) {
            data[i - countyDataValues] = entries[i].toInt();
        }

        // get state of this item
        CovidDataTreeItem *state = rootItem.getChild(nameOfState);

        // if state is not already added it is an unusual case like a cruise ship.
        if(!state) {
            // so this unusual dataset will be added on the state level
            CovidDataTreeItem newStateItem(nameOfState);
            rootItem.addChild(fips, newStateItem);

            // get valid ptr to previously added item
            state = rootItem.getChild(nameOfState);
        }

        // in some cases the County name is empty => then data w2ill be added to the state
        CovidDataTreeItem *dataItemPtr;
        if(nameOfCounty.isEmpty()) {
            dataItemPtr = state;
        } else {
            // get ptr to county
            CovidDataTreeItem *county = state->getChild(nameOfCounty);

            // add county if it is not already in data
            if(!county) {
                CovidDataTreeItem newCountyItem(nameOfCounty);
                state->addChild(fips, newCountyItem);

                // get valid ptr to previously added item
                county = state->getChild(nameOfCounty);
            }

            dataItemPtr = county;
        }

        if(fileType == FileType::Cases) {
            dataItemPtr->setLocation(QPointF((360.0 + lon) * 100, lat * 100));
            dataItemPtr->getCaseData().casesCumulated       = data;
            dataItemPtr->getCaseData().cases                = calculateIncrease(data);
            dataItemPtr->getCaseData().casesSevenDayAverage = calculateAveragedIncrease(dataItemPtr->getCaseData().cases.series);
        } else if(fileType == FileType::Deaths) {
            dataItemPtr->getCaseData().deathsCumulated      = data;
            dataItemPtr->getCaseData().deaths               = calculateIncrease(data);
            dataItemPtr->getCaseData().population           = entries[11].toInt();
            dataItemPtr->getCaseData().startDate            = startDate;
        }
    }

    return true;
}

int UsaDataReader::getFips(QString uid, QString fips)
{
    if(fips.isEmpty()) {
        return (uid.toInt() % 100000);
    } else {
        if(fips.toFloat() < 1000.0f) {
            return 0;
        } else {
            return static_cast<int>(fips.toFloat());
        }
    }
}

void UsaDataReader::calculateCaseDataSums()
{
    // if the root item has children sum its casedata
    if(rootItem.getChildren().size() > 0) {
        CaseData &rootItemCases = rootItem.getCaseData();

        // init the case vectors with the size of the first child items data size
        rootItemCases.fill(0, dataValues);
        rootItemCases.population = 0;
        rootItemCases.startDate  = startDate;

        // iterate over all states, sum up their population and case data
        for(CovidDataTreeItem &stateItem : rootItem.getChildren()) {
            CaseData &stateItemCases = stateItem.getCaseData();

            // before adding the state data, add the county data to the states.
            if(stateItem.getChildren().size() > 0) {
                // init state data
                stateItemCases.fill(0, dataValues);
                stateItemCases.population = 0;
                stateItemCases.startDate  = startDate;

                // add county data
                for(CovidDataTreeItem &countyItem : stateItem.getChildren()) {
                    stateItemCases.population += countyItem.getCaseData().population;
                    addCaseData(stateItemCases, countyItem.getCaseData());
                }
            }

            // add state data
            rootItemCases.population += stateItemCases.population;
            addCaseData(rootItemCases, stateItemCases);
        }
    }
}

QDate UsaDataReader::getStartDateOfDataSet()
{
    //open file
    QFile file;
    file.setFileName(deathsUsaFile);
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

CovidDataTreeItem *UsaDataReader::getItemByFips(int stateFips, int countyFips)
{
    CovidDataTreeItem *item = rootItem.getChild(stateFips);
    if(item) {
        if(countyFips == 0) {
            return item;
        } else {
            return item->getChild(countyFips);
        }
    }

    return nullptr;
}

bool UsaDataReader::loadMapData()
{
    // get data from shape file
    QVector<MapRegion> regions;

    // load data in try-catch block since there might be unusual errors.
    try {
        // read shape file
        if(!readShapeFileData(mapShapeFile, regions, true)) {
            return false;
        }

        // read dbf file
        if(!readDatabaseFileUsa(mapShapeFile, regions)) {
            return false;
        }

    } catch (QString &s) {
        qDebug() << s;
    } catch(...) {
        qDebug() << "Other exception";
    }

    // merge the mapregion data to the CovidDataTreeItem data
    mergeMapData(regions);

    return true;
}

bool UsaDataReader::mergeMapData(QVector<MapRegion> &regions)
{
    for(MapRegion &region : regions) {
        // finde corresponding item in coviddata tree
        CovidDataTreeItem *item = getItemByFips(region.stateFips.toInt(),
                                                (region.stateFips + region.countyFips).toInt());

        // if item is not found we can not add the data
        if(!item) {
            continue;
        }

        // add additional data
        item->setItemName(region.name);

        CovidDataTreeItem *itemParent = getItemByFips(region.stateFips.toInt(), 0);
        if(itemParent) {
            item->setItemDesignation(itemParent->getItemNameAlt());
        }

        // calculate the painterpath
        QPainterPath path;
        // add polygons to painter path
        for(const QPolygonF &polygon : qAsConst(region.polygons)) {
            path.addPolygon(polygon);
        }

        item->createAreaItem(path.simplified());
    }

    return true;
}
