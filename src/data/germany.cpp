#include "germany.h"

#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

Germany::Germany()
{
    init();
}

Germany::~Germany()
{

}

bool Germany::loadData(const QString &folder)
{
    qDebug() << "Loading data: " << folder;

    return true;
}

void Germany::init()
{
    initStates();
    initDistricts();
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
    data.countryCode = static_cast<CountryCode>(root.value("id").toInt());
    data.name = root.value("name").toString();
    data.code = root.value("code").toString();

    // get state data
    QJsonArray states = root.value("states").toArray();
    for(const QJsonValue &v : states) {
        State s;
        s.id = static_cast<DE_Code>(v.toObject().value("id").toInt());
        s.name = v.toObject().value("name").toString();
        s.code = v.toObject().value("code").toString();

        data.states.append(s);
    }
}

void Germany::initDistricts()
{
    // TODO: check data sources
//    // init districts from internal csv file
//    QString districtFile = ":/arcgis/kreisgrenzen/Kreisgrenzen_2018_mit_Einwohnerzahl.csv";
//    if(!QFile::exists(districtFile)) {
//        qDebug() << "File not found";
//    }
}
