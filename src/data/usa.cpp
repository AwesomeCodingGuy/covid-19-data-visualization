#include "usa.h"
#include "../utils/utility.h"

#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace usa {

Uid::Uid()
    : uid(0)
    , code3(0)
    , stateId(0)
    , fips(0)
{

}

Uid::Uid(int uid)
{
    this->uid = uid;
    fips = uid % 1000;
    uid /= 1000;
    stateId = uid % 100;
    uid /= 100;
    code3 = fips;
}

Uid::Uid(int code3, int stateId, int fips)
    : code3(code3)
    , stateId(stateId)
    , fips(fips)
{
    uid = code3 * 100000 + stateId * 1000 + fips;
}

Uid::Uid(int uid, int code3, int stateId, int fips)
    : code3(code3)
    , stateId(stateId)
    , fips(fips)
{
    if(uid < 1000) {
        this->uid = code3 * 100000 + stateId * 1000 + fips;
    } else {
        this->uid = uid;
    }
}

Usa::Usa()
{
    initStates();
}

Usa::~Usa()
{

}

County* Usa::getCountyByName(const QString &name)
{
    for(auto it = states.begin(); it != states.end(); ++it) {
        for(auto itC = it->counties.begin(); itC != it->counties.end(); ++itC) {
            if(itC->name == name) {
                return itC;
            }
        }
    }

    return nullptr;
}

County* Usa::getCountyByName(State &state, const QString &name)
{
    for(auto it = state.counties.begin(); it != state.counties.end(); ++it) {
        if(it->name == name) {
            return it;
        }
    }

    return nullptr;
}

State* Usa::getStateByName(const QString &name)
{
    for(auto it = states.begin(); it != states.end(); ++it) {
        if(it->name == name) {
            return it;
        }
    }

    return nullptr;
}

QString Usa::getNameOfCounty(const Uid uid) const
{
    for(auto it = states.begin(); it != states.end(); ++it) {
        if(it->uid.stateId == uid.stateId) {
            for(auto itC = it->counties.begin(); itC != it->counties.end(); ++itC) {
                if(itC->uid.uid == uid.uid) {
                    return itC->name;
                }
            }
        }
    }

    return QString();
}

QString Usa::getNameOfState(const Uid uid) const
{
    for(auto it = states.begin(); it != states.end(); ++it) {
        if(it->uid.stateId == uid.stateId) {
            if(it->uid.uid == uid.uid) {
                return it->name;
            }
        }
    }

    return QString();
}

const QVector<QDateTime> Usa::getTimestamps() const
{
    return timestamps;
}

const CaseData* Usa::getCaseDataByUid(const Uid &uid) const
{
    if(this->uid.uid == uid.uid) {
        return &this->data;
    } else {
        for(auto it = states.begin(); it != states.end(); ++it) {
            if(it->uid.stateId == uid.stateId) {
                if(it->uid.uid == uid.uid) {
                    return &it->data;
                } else {
                    for(auto itC = it->counties.begin(); itC != it->counties.end(); ++itC) {
                        if(itC->uid.uid == uid.uid) {
                            return &itC->data;
                        }
                    }
                }
            }
        }
    }

    return nullptr;
}

bool Usa::loadData(const QString &folder)
{
    // read confirmed cases
    QString confirmedGlobal = QString(folder).append("/csse_covid_19_data/csse_covid_19_time_series/time_series_covid19_confirmed_US.csv");
    if(!readCsvByCounty(confirmedGlobal, FileType::Cases)) {
        return false;
    }
    // read confirmed deaths
    QString deathsGlobal = QString(folder).append("/csse_covid_19_data/csse_covid_19_time_series/time_series_covid19_deaths_US.csv");
    if(!readCsvByCounty(deathsGlobal, FileType::Deaths)) {
        return false;
    }

    // sum up all couny and state values
    calculateSummedValues();

    return true;
}

void Usa::initStates()
{
    // init states from internal json file
    QString stateFile = ":/us_states.json";
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
    uid = Uid(root.value("id").toInt(), 0, 0);
    name = root.value("name").toString();
    code = root.value("code").toString();

    // get state data
    QJsonArray stateValues = root.value("states").toArray();
    for(const QJsonValue &v : stateValues) {
        State s;
        s.name = v.toObject().value("name").toString();
        s.code = v.toObject().value("code").toString();
        s.uid = Uid(uid.code3, v.toObject().value("fips").toInt(), 0);

        states.append(s);
    }
}

bool Usa::readCsvByCounty(const QString &filename, FileType fileType)
{
    // check if file exists
    if(!QFile::exists(filename)) {
        return false;
    }

    // open file
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // process first line (headline)
    QList<QByteArray> captions = file.readLine().split(',');
    const int countyDataValues = fileType == FileType::Cases ? 11 : 12;
    int dataValues = captions.size() - countyDataValues;

    // get timestamps
    // we need this only once
    if(fileType == FileType::Cases) {
        timestamps.resize(dataValues);
        for(int i = countyDataValues; i < captions.size(); ++i) {
            auto dateParts = captions[i].split('/');
            timestamps[i - countyDataValues] = QDateTime(QDate(2000 + dateParts[2].toInt(),
                                                               dateParts[0].toInt(),
                                                               dateParts[1].toInt()),
                                                         QTime());
        }
    }

    // get all countries and provinces line by line
    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        escapeCommasInStrings(line);
        auto entries = line.split(',');
        resolveEscapes(entries);

        // get data
        int uid = entries[0].toInt();
        int code3 = entries[3].toInt();
        int fips = 0;
        if(entries[4].isEmpty()) {
            if(uid > 1000) {
                fips = uid % 1000;
            } else {
                fips = 0;
            }
        } else {
            fips = static_cast<int>(entries[4].toFloat()) % 1000;
        }

        int population = 0;
        if(fileType == FileType::Deaths) {
            population = entries[11].toInt();
        }

        QString nameOfCounty = entries[5];
        QString nameOfState = entries[6];
        Location location;
        location.latitude = entries[8].toDouble();
        location.longitude = entries[9].toDouble();
        QVector<int> data;
        data.resize(dataValues);
        for(int i = countyDataValues; i < entries.size(); ++i) {
            data[i - countyDataValues] = entries[i].toInt();
        }

        // get ptr to State
        State *state = getStateByName(nameOfState);

        // add state if it is not already in vector
        // this can only happen in unusual cases like cruise ships etc.
        if(!state) {
            State newState;
            newState.name = nameOfState;
            newState.uid = Uid(uid);
            newState.population = population;
            states.push_back(newState);

            // get valid ptr to previously added state
            state = getStateByName(nameOfState);
        }

        // if name of county is empty, add data to the state
        CaseData *caseDataPtr;
        if(nameOfCounty.isEmpty()) {
            state->location = location;
            state->population = population;

            caseDataPtr = &state->data;
        } else {
            // get ptr to county
            County *county = getCountyByName(*state, nameOfCounty);

            // add county if it is not already in vector
            if(!county) {
                County newCounty;
                newCounty.name = nameOfCounty;
                newCounty.location = location;
                newCounty.uid = Uid(uid, code3, state->uid.stateId, fips);
                newCounty.population = population;
                state->counties.push_back(newCounty);

                // get valid ptr to previously added county
                county = getCountyByName(*state, nameOfCounty);
            }

            caseDataPtr = &county->data;
        }

        if(fileType == FileType::Cases) {
            caseDataPtr->casesCumulated = data;
            caseDataPtr->cases = calculateIncrease(data);
            caseDataPtr->casesSevenDayAverage = calculateAveragedIncrease(caseDataPtr->cases);
        } else if(fileType == FileType::Deaths) {
            caseDataPtr->deathsCumulated = data;
            caseDataPtr->deaths = calculateIncrease(data);
        }
    }

    return true;
}

void Usa::calculateSummedValues()
{
    // init data
    data.fill(0, timestamps.size());

    // iterate over all states and add up their population and case data
    for(State &state : states) {
        // skip adding if there are no counties
        if(state.counties.size() > 0) {
            // init data
            state.data.fill(0, timestamps.size());

            // fill data from containing counties
            state.population = 0;
            for(const County &county : state.counties) {
                state.population += county.population;
                addCaseData(state.data, county.data);
            }
        }

        // add state data
        population += state.population;
        addCaseData(data, state.data);
    }
}

} // namespace usa
