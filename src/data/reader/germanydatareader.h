#ifndef GERMANYDATAREADER_H
#define GERMANYDATAREADER_H

#include <QMap>
#include <QList>
#include <QByteArray>
#include <QVector>
#include <QString>
#include <QDate>

#include "../mapregion.h"
#include "../coviddatatreeitem.h"

class GermanyDataReader
{
public:
    GermanyDataReader() = delete;
    GermanyDataReader(const QString &folder, CovidDataTreeItem &dataItem);
    ~GermanyDataReader();

    bool validateFiles() const;
    bool loadData();

private:
    void initStates();
    void initDistricts();
    bool readCasesByState();
    bool readCasesByAgs();
    void getCaptionsAndData(const QString &file,
                            QList<QByteArray> &captions,
                            QVector<QVector<int>> &data);
    CovidDataTreeItem *getItemByAgs(int ags);
    QDate getStartDateOfDataSet();

    bool loadMapData();
    bool mergeMapData(QVector<MapRegion> &regions);

    // variables
    QString stateInitFile;
    QString casesByStateFile;
    QString deathsByStateFile;
    QString casesByAgsFile;
    QString deathsByAgsFile;
    QString agsFile;
    QString mapShapeFile;

    CovidDataTreeItem &rootItem;

    QMap<QString, int> stateCodeMap;
    QDate startDate;
};

#endif // GERMANYDATAREADER_H
