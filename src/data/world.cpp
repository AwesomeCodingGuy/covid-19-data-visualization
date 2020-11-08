#include "world.h"
#include "../utils/utility.h"

#include <QFile>
#include <QDebug>
#include <QDateTime>

namespace world {

World::World()
{

}

World::~World()
{

}

Province* World::getProvinceByName(const QString &name)
{
    for(auto it = countries.begin(); it != countries.end(); ++it) {
        for(auto itP = it->provinces.begin(); itP != it->provinces.end(); ++itP) {
            if(itP->name == name) {
                return itP;
            }
        }
    }

    return nullptr;
}

Province* World::getProvinceByName(Country &country, QString &name)
{
    for(auto it = country.provinces.begin(); it != country.provinces.end(); ++it) {
        if(it->name == name) {
            return it;
        }
    }

    return nullptr;
}

Country* World::getCountryByName(const QString &name)
{
    for(auto it = countries.begin(); it != countries.end(); ++it) {
        if(it->name == name) {
            return it;
        }
    }

    return nullptr;
}

bool World::getCaseDataByName(const QString &country, const QString &province,
                              CaseData &caseData, QVector<QDateTime> &timestamps)
{
    if(country == this->name) {
        caseData = this->data;
        timestamps = this->timestamps;
        return true;
    } else {
        for(const Country &c : this->countries) {
            if(country == c.name) {
                if(province.isEmpty()) {
                    caseData = c.data;
                    timestamps = this->timestamps;
                    return true;
                } else {
                    for(const Province &p : c.provinces) {
                        if(province == p.name) {
                            caseData = p.data;
                            timestamps = this->timestamps;
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool World::loadData(const QString &folder)
{
    // read confirmed cases
    QString confirmedGlobal = QString(folder).append("/csse_covid_19_data/csse_covid_19_time_series/time_series_covid19_confirmed_global.csv");
    if(!readCsvByCountry(confirmedGlobal, FileType::Cases)) {
        return false;
    }
    // read confirmed cases
    QString deathsGlobal = QString(folder).append("/csse_covid_19_data/csse_covid_19_time_series/time_series_covid19_deaths_global.csv");
    if(!readCsvByCountry(deathsGlobal, FileType::Deaths)) {
        return false;
    }

    calculateTotalValues();

    return true;
}

bool World::readCsvByCountry(const QString &filename, FileType fileType)
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
    int dataValues = captions.size() - 4;

    // get timestamps
    // we need this only once
    if(fileType == FileType::Cases) {
        name = "World";
        timestamps.resize(dataValues);
        for(int i = 4; i < captions.size(); ++i) {
            auto dateParts = captions[i].split('/');
            timestamps[i - 4] = QDateTime(QDate(2000 + dateParts[2].toInt(),
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
        QString nameOfProvince = entries[0];
        QString nameOfCountry = entries[1];
        Location location;
        location.latitude = entries[2].toFloat();
        location.longitude = entries[3].toFloat();
        QVector<int> data;
        data.resize(dataValues);
        for(int i = 4; i < entries.size(); ++i) {
            data[i - 4] = entries[i].toInt();
        }

        // get ptr to country
        Country *country = getCountryByName(nameOfCountry);

        // add country if it is not already in vector
        if(!country) {
            Country newCountry;
            newCountry.name = nameOfCountry;
            newCountry.location = location;
            countries.push_back(newCountry);

            // get valid ptr to country
            country = getCountryByName(nameOfCountry);
        }

        // if province is empty add data to country, else add the data to new province
        if(nameOfProvince.isEmpty()) {
            if(fileType == FileType::Cases) {
                country->data.casesCumulated = data;
                country->data.cases = calculateIncrease(data);
                country->data.casesSevenDayAverage = calculateAveragedIncrease(country->data.cases);
            } else {
                country->data.deathsCumulated = data;
                country->data.deaths = calculateIncrease(data);
            }
        } else {
            Province *province = getProvinceByName(*country, nameOfProvince);

            // add province if it is not already in vector
            if(!province) {
                Province newProvince;
                newProvince.name = nameOfProvince;
                newProvince.location = location;
                country->provinces.push_back(newProvince);

                // get valid ptr to country
                province = getProvinceByName(*country, nameOfProvince);
            }

            if(fileType == FileType::Cases) {
                province->data.casesCumulated = data;
                province->data.cases = calculateIncrease(data);
                province->data.casesSevenDayAverage = calculateAveragedIncrease(province->data.cases);
            } else {
                province->data.deathsCumulated = data;
                province->data.deaths = calculateIncrease(data);
            }

        }
    }

    return true;
}

void World::calculateTotalValues()
{
    // init world data
    data.fill(0, timestamps.size());

    // find all countries with provinces and calculate total values
    for(Country &country : countries) {
        // this is onyl relevent if the country contains provinces
        if(country.provinces.size() > 0) {
            // if there is data for the heartland, move the data to a new province first
            if(country.data.casesCumulated.size() > 0) {
                Province province;
                province.name = QString(country.name).append(" (Heartland)");
                province.location = country.location;
                province.data = country.data;

                country.location = Location();
                country.provinces.push_back(province);
            }

            country.data.fill(0, timestamps.size());

            // add data vectors of provinces to country vector
            for(const Province &province : country.provinces) {
                addCaseData(country.data, province.data);
            }
        }

        // add county data
        addCaseData(data, country.data);
    }
}

} // namespace world
