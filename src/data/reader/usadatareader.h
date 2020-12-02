#ifndef USADATAREADER_H
#define USADATAREADER_H

#include <QList>
#include <QByteArray>
#include <QVector>
#include <QString>
#include <QDate>

#include "../mapregion.h"
#include "../coviddatatreeitem.h"

class UsaDataReader
{
    enum class FileType {
        Cases,
        Deaths
    };

public:
    UsaDataReader() = delete;
    UsaDataReader(const QString &folder, CovidDataTreeItem &dataItem);
    ~UsaDataReader();

    bool validateFiles() const;
    bool loadData();

    bool loadMapData();
    bool mergeMapData(QVector<MapRegion> &regions);

private:
    void initStates();
    bool readCsvFileByCounty(const QString &filename, FileType fileType);
    int getFips(QString uid, QString fips);
    void calculateCaseDataSums();
    void calculateAndAddChildrenCaseData(CovidDataTreeItem &item);
    QDate getStartDateOfDataSet();

    CovidDataTreeItem *getItemByFips(int stateFips, int countyFips);

    // variables
    QString usaStatesFile;
    QString confirmedUsaFile;
    QString deathsUsaFile;
    QString mapShapeFile;

    CovidDataTreeItem &rootItem;
    QDate startDate;
    int dataValues;
};

#endif // USADATAREADER_H
