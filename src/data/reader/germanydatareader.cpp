#include "germanydatareader.h"

#include "../constants.h"
#include "../../utils/utility.h"
#include "../../utils/shapelibutils.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QPainterPath>

GermanyDataReader::GermanyDataReader(const QString &folder, CovidDataTreeItem &dataItem)
    : rootItem(dataItem)
{
    stateInitFile       = constants::germanyStateJson;
    mapShapeFile        = constants::germanyShapeFile;
    casesByStateFile    = QString(folder).append(constants::casesByState);
    deathsByStateFile   = QString(folder).append(constants::deathsByState);
    casesByAgsFile      = QString(folder).append(constants::casesByAgs);
    deathsByAgsFile     = QString(folder).append(constants::deathsByAgs);
    agsFile             = QString(folder).append(constants::ags);
}

GermanyDataReader::~GermanyDataReader()
{

}

bool GermanyDataReader::validateFiles() const
{
    return (QFile::exists(stateInitFile)
         && QFile::exists(mapShapeFile)
         && QFile::exists(casesByStateFile)
         && QFile::exists(deathsByStateFile)
         && QFile::exists(casesByAgsFile)
         && QFile::exists(deathsByAgsFile)
         && QFile::exists(agsFile));
}

bool GermanyDataReader::loadData()
{
    // check existence of input files
    if(!validateFiles()) {
        return false;
    }

    // set new item to clear existing data.
    rootItem = CovidDataTreeItem();

    // get the date of first dataset
    startDate = getStartDateOfDataSet();

    // read state data
    if(!readCasesByState()) {
        return false;
    }

    // read district data
    if(!readCasesByAgs()) {
        return false;
    }

    // read shapefile
    if(!loadMapData()) {
        return false;
    }

    return true;
}

void GermanyDataReader::initStates()
{
    // open file
    QFile stateFile;
    stateFile.setFileName(stateInitFile);
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
        rootItem.addChild(v.toObject().value("id").toInt(), newStateItem);

        // insert code and id combination for finding states in csv file data
        stateCodeMap.insert(v.toObject().value("code").toString(),
                            v.toObject().value("id").toInt());
    }
}

void GermanyDataReader::initDistricts()
{
    // open file
    QFile file;
    file.setFileName(agsFile);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // read file content and close file
    QString jsonContent = file.readAll();
    file.close();

    // get json root object
    QJsonDocument doc = QJsonDocument::fromJson(jsonContent.toUtf8());
    QJsonObject jsonRoot = doc.object();

    // iterate over ags objects
    for(QString agsStr : jsonRoot.keys()) {
        // get data from file
        QJsonObject district = jsonRoot.value(agsStr).toObject();
        const int ags = agsStr.toInt();
        const int stateKey = ags / 1000;
        const int districtKey = ags % 1000;

        // Do not add Districts for States, e.g. Hamburg and Berlin
        if(districtKey == 0) {
            continue;
        }

        // add new item
        CovidDataTreeItem newItem;
        newItem.setItemNameAlt(district.value("name").toString());
        CovidDataTreeItem *state = rootItem.getChild(stateKey);
        state->addChild(districtKey, newItem);
    }
}

bool GermanyDataReader::readCasesByState()
{
    // init the state hierachy
    initStates();

    // get data from files
    QList<QByteArray> casesCaptions;
    QVector<QVector<int>> casesData;
    getCaptionsAndData(casesByStateFile, casesCaptions, casesData);

    QList<QByteArray> deathsCaptions;
    QVector<QVector<int>> deathsData;
    getCaptionsAndData(deathsByStateFile, deathsCaptions, deathsData);

    // set data in root items children
    for(int i = 0; i < (casesData.size() - 1); ++i) {
        CovidDataTreeItem *state = rootItem.getChild(stateCodeMap[casesCaptions[i + 1]]);
        if(!state) {
            qDebug() << "Error";
            continue;
        }

        CaseData newData;
        newData.casesCumulated          = casesData[i];
        newData.deathsCumulated         = deathsData[i];
        newData.cases                   = calculateIncrease(casesData[i]);
        newData.deaths                  = calculateIncrease(deathsData[i]);
        newData.casesSevenDayAverage    = calculateAveragedIncrease(newData.cases.series);
        newData.startDate               = this->startDate;

        state->setCaseData(newData);
    }

    // set data in root item
    CaseData newData;
    newData.casesCumulated          = casesData.last();
    newData.deathsCumulated         = deathsData.last();
    newData.cases                   = calculateIncrease(casesData.last());
    newData.deaths                  = calculateIncrease(deathsData.last());
    newData.casesSevenDayAverage    = calculateAveragedIncrease(newData.cases.series);
    newData.startDate               = this->startDate;

    rootItem.setCaseData(newData);

    return true;
}

bool GermanyDataReader::readCasesByAgs()
{
    // init the districts hierachy
    initDistricts();

    // get data from files
    QList<QByteArray> casesCaptions;
    QVector<QVector<int>> casesData;
    getCaptionsAndData(casesByAgsFile, casesCaptions, casesData);

    QList<QByteArray> deathsCaptions;
    QVector<QVector<int>> deathsData;
    getCaptionsAndData(deathsByAgsFile, deathsCaptions, deathsData);

    // set data in districts, size - 2 => skipping first and last column
    for(int i = 0; i < (casesCaptions.size() - 2); ++i) {
        // get district from ags number - nullptr if not found
        CovidDataTreeItem *district = getItemByAgs(casesCaptions[i + 1].toInt());

        // in case the district is not found skip this dataset
        if(!district) {
            continue;
        }

        // set data in item
        CaseData newData;
        newData.casesCumulated          = casesData[i];
        newData.deathsCumulated         = deathsData[i];
        newData.cases                   = calculateIncrease(casesData[i]);
        newData.deaths                  = calculateIncrease(deathsData[i]);
        newData.casesSevenDayAverage    = calculateAveragedIncrease(newData.cases.series);
        newData.startDate               = this->startDate;

        district->setCaseData(newData);
    }

    return true;
}

void GermanyDataReader::getCaptionsAndData(const QString &filename,
                                           QList<QByteArray> &captions,
                                           QVector<QVector<int>> &data)
{
    //open file
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // get captions
    captions = file.readLine().split(',');

    // init data vector
    data.clear();
    data.resize(captions.size() - 1);

    // TODO: check how many lines can be read and init inner vectors
    // read lines of data
    // the first column is the date, which will be ignored
    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        auto entries = line.split(',');

        for(int i = 1; i < entries.size(); ++i) {
            data[i - 1].push_back(entries[i].toInt());
        }
    }

    file.close();
}

CovidDataTreeItem *GermanyDataReader::getItemByAgs(int ags)
{
    const int stateKey = ags / 1000;
    const int districtKey = ags % 1000;

    CovidDataTreeItem *item = rootItem.getChild(stateKey);
    if(item) {
        if(districtKey == 0) {
            return item;
        } else {
            return item->getChild(districtKey);
        }
    }
    return nullptr;
}

QDate GermanyDataReader::getStartDateOfDataSet()
{
    //open file
    QFile file;
    file.setFileName(casesByStateFile);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // get first entry of second line in file
    file.readLine();
    QByteArray dateString = file.readLine().split(',')[0];

    // close file
    file.close();

    // return date
    return QDateTime::fromString(dateString, Qt::DateFormat::ISODate).date();
}

bool GermanyDataReader::loadMapData()
{
    // get data from shape file
    QVector<MapRegion> regions;

    // load data in try-catch block since there might be unusual errors.
    try {
        // read shape file
        if(!readShapeFileData(mapShapeFile, regions)) {
            return false;
        }

        // read dbf file
        if(!readDatabaseFileGermany(mapShapeFile, regions)) {
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

bool GermanyDataReader::mergeMapData(QVector<MapRegion> &regions)
{
    for(MapRegion &region : regions) {
        // finde corresponding item in coviddata tree
        CovidDataTreeItem *item = getItemByAgs(region.ags);

        // if item is not found we can not add the data
        if(!item) {
            continue;
        }

        // add additional data
        item->setItemName(region.name);
        item->setItemDesignation(region.bez);
        item->getCaseData().population = region.ewz;

        // calculate the painterpath
        QPainterPath path;
        // add polygons to painter path
        for(const QPolygonF &polygon : region.polygons) {
            path.addPolygon(polygon);
        }

        item->createAreaItem(path.simplified());
    }

    return true;
}
