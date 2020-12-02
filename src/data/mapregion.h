#ifndef MAPREGION_H
#define MAPREGION_H

#include <QString>
#include <QVector>
#include <QPolygonF>

struct MapRegion
{
    QString name;
    QVector<QPolygonF> polygons;
    QString agsStr;
    int ags;
    int parts;
    int ewz;
    QString bez;
    QString stateFips;
    QString countyFips;
};

struct MapRegionUs
{
    QString name;
    QVector<QPolygonF> polygons;
    QString stateFips;
    QString countyFips;
    int parts;
};

#endif // MAPREGION_H
