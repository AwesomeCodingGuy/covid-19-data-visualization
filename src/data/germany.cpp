#include "germany.h"
#include "constants.h"
#include "../utils/utility.h"

#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace germany {

Germany::Germany()
{
    init();
}

Germany::~Germany()
{

}

QString Germany::getName(int ags)
{
    return getDistrictByAgs(ags)->name;
}

State* Germany::getStateByName(const QString &state)
{
    if(!country.states.empty()) {
        for(auto it = country.states.begin(); it != country.states.end(); ++it) {
            if(it->name == state)
                return it;
        }
    }

    return nullptr;
}

State* Germany::getStateByCode(const QString &code)
{
    if(!country.states.empty()) {
        for(auto it = country.states.begin(); it != country.states.end(); ++it) {
            if(it->code == code)
                return it;
        }
    }

    return nullptr;
}

District* Germany::getDistrictByAgs(const QString &ags)
{
    return getDistrictByAgs(ags.toInt());
}

District* Germany::getDistrictByAgs(int ags)
{
    if(country.districtMap.contains(ags)) {
        return country.districtMap.value(ags);
    } else {
        return nullptr;
    }
}

const Country& Germany::getCountryData() const
{
    return country;
}

bool Germany::getCaseDataByCode(QString code,
                                CaseData &caseData,
                                QVector<QDateTime> &timestamps)
{
    if(code == country.code) {
        caseData = country.data;
        timestamps = country.timestamps;
        return true;
    } else {
        State *state = getStateByCode(code);
        if(state) {
            caseData = state->data;
            timestamps = country.timestamps;
            return true;
        } else {
            District *district = getDistrictByAgs(code);
            if(district) {
                caseData = district->data;
                timestamps = country.timestamps;
                return true;
            }
        }
    }

    return false;
}

const CaseData *Germany::getCaseDataByAgs(const int ags)
{
    return &getDistrictByAgs(ags)->data;
}

const QVector<QDateTime> Germany::getTimestamps() const
{
    return country.timestamps;
}

bool Germany::loadData(const QString &folder)
{
    // read cases by state
    QString casesByStateFile = QString(folder).append(constants::casesByState);
    if(!readCsvByState(casesByStateFile, FileType::Cases)) {
        return false;
    }
    // read deaths by state
    QString deathsByStateFile = QString(folder).append(constants::deathsByState);
    if(!readCsvByState(deathsByStateFile, FileType::Deaths)) {
        return false;
    }

    // read ags file
    QString agsFile = QString(folder).append(constants::ags);
    if(!readJsonAgs(agsFile)) {
        return false;
    }

    // read cases by ags
    QString casesByAgsFile = QString(folder).append(constants::casesByAgs);
    if(!readCsvByAgs(casesByAgsFile, FileType::Cases)) {
        return false;
    }

    // read deaths by ags
    QString deathsByAgsFile = QString(folder).append(constants::deathsByAgs);
    if(!readCsvByAgs(deathsByAgsFile, FileType::Deaths)) {
        return false;
    }

    return true;
}

void Germany::init()
{
    initStates();
}

void Germany::initStates()
{
    // init states from internal json file
    QString stateFile = ":/de_bundeslaender.json";
    if(!QFile::exists(stateFile)) {
        qDebug() << "File not found";
    }

    // open file
    QFile file;
    file.setFileName(stateFile);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // read file content and close file
    QString jsonContent = file.readAll();
    file.close();

    // get json root object
    QJsonDocument doc = QJsonDocument::fromJson(jsonContent.toUtf8());
    QJsonObject root = doc.object();

    // init country data
    country.id = static_cast<DE_Code>(root.value("id").toInt());
    country.name = root.value("name").toString();
    country.code = root.value("code").toString();

    // get state data
    QJsonArray states = root.value("states").toArray();
    for(const QJsonValue &v : states) {
        State s;
        s.id = static_cast<DE_Code>(v.toObject().value("id").toInt());
        s.name = v.toObject().value("name").toString();
        s.code = v.toObject().value("code").toString();

        country.states.append(s);
    }
}

bool Germany::readJsonAgs(QString filename)
{
    // check if file exists
    if(!QFile::exists(filename)) {
        return false;
    }

    // open file
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // read file content and clsoe file
    QString json = file.readAll();
    file.close();

    // convert raw string data to a JSON object
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject rootObj = doc.object();

    // iterate over ags objects
    for(QString str : rootObj.keys()) {
        // get the district data values from the json object
        int ags = str.toInt();
        QJsonObject obj = rootObj.value(str).toObject();
        QString state = obj.value("state").toString();
        QString name = obj.value("name").toString();

        // build a new district object
        District d;
        d.ags = ags;
        d.name = name;

        // add district to its parent state, given from the json data
        State* s = getStateByName(state);
        s->districts.insert(ags, d);

        // add a pointer to the district in the country's ags map
        country.districtMap.insert(ags, &s->districts[ags]);
    }

    return true;
}

bool Germany::readCsvByState(QString filename, FileType fileType)
{
    // check if file exists
    if(!QFile::exists(filename)) {
        return false;
    }

    // open file
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // process first Line (captions for columns)
    QList<QByteArray> captions = file.readLine().split(',');
    captions.removeFirst();
    const int dataSize = captions.size();
    if(dataSize != 17) // 17 = (16 States + Sum)
        return false;
    QVector<QVector<int>> data;
    data.resize(dataSize);

    // read lines of data
    // the first column will be ignored, since it contains only time data, which will be collected later
    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        auto entries = line.split(',');

        for(int i = 1; i < entries.size(); ++i) {
            data[i - 1].push_back(entries[i].toInt());
        }
    }

    // we are done with the file
    file.close();

    // add the data to the covid data struct
    for(int i = 0; i < dataSize - 1; ++i) {
        State *s = getStateByCode(captions[i]);
        if(fileType == FileType::Cases) {
            s->data.casesCumulated = data[i];
            s->data.cases = calculateIncrease(data[i]);
            s->data.casesSevenDayAverage = calculateAveragedIncrease(s->data.cases);
        } else {
            s->data.deathsCumulated = data[i];
            s->data.deaths = calculateIncrease(data[i]);
        }
    }
    if(fileType == FileType::Cases) {
        country.data.casesCumulated = data[dataSize - 1];
        country.data.cases = calculateIncrease(data[dataSize - 1]);
        country.data.casesSevenDayAverage = calculateAveragedIncrease(country.data.cases);
    } else {
        country.data.deathsCumulated = data[dataSize - 1];
        country.data.deaths = calculateIncrease(data[dataSize - 1]);
    }

    return true;
}

bool Germany::readCsvByAgs(QString filename, FileType fileType)
{
    // check if file exists
    if(!QFile::exists(filename)) {
        return false;
    }

    // open file
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // process first Line (ags for columns)
    QList<QByteArray> captions = file.readLine().split(',');
    captions.removeFirst();
    QVector<QDateTime> timeStamps;
    QVector<QVector<int>> data;
    data.resize(captions.size() - 1); // skipping total values, we already have them

    // read data lines
    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        auto entries = line.split(',');

        QDateTime dt = QDateTime::fromString(entries[0], Qt::DateFormat::ISODate);
        timeStamps.append(dt);

        for(int i = 1; i < entries.size() - 1; ++i) {
            data[i - 1].push_back(entries[i].toInt());
        }
    }

    // we are done with the file
    file.close();

    // add the data to the districts' data struct
    for(int i = 0; i < captions.size() - 1; ++i) {
        District *d = getDistrictByAgs(captions[i].toInt());
        if(fileType == FileType::Cases) {
            d->data.casesCumulated = data[i];
            d->data.cases = calculateIncrease(data[i]);
            d->data.casesSevenDayAverage = calculateAveragedIncrease(d->data.cases);
        } else {
            d->data.deathsCumulated = data[i];
            d->data.deaths = calculateIncrease(data[i]);
        }
    }

    // add the timestamps, we only want to do this once
    if(fileType == FileType::Cases) {
        country.timestamps = timeStamps;
    }

    return true;
}

} // namespace germany
