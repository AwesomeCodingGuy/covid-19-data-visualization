#ifndef WORLDDATAREADER_H
#define WORLDDATAREADER_H

#include <QString>
#include <QDate>

#include "../coviddatatreeitem.h"

class WorldDataReader
{
    enum class FileType {
        Cases,
        Deaths
    };

public:
    WorldDataReader() = delete;
    WorldDataReader(const QString &folder, CovidDataTreeItem &dataItem);
    ~WorldDataReader();

    bool validateFiles() const;
    bool loadData();

private:
    bool readCsvFileByCounty(const QString &filename, FileType fileType);
    void calculateTotalvalues();
    QDate getStartDateOfDataSet();

    QString confirmedGlobalFile;
    QString deathsGlobalFile;

    CovidDataTreeItem &rootItem;
    QDate startDate;
    int dataValues;
};

#endif // WORLDDATAREADER_H
