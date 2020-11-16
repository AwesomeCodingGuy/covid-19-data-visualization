#ifndef SHAPELIBUTILS_H
#define SHAPELIBUTILS_H

#include <QDebug>
#include <QString>
#include <QVector>
#include <QPolygonF>

#include "shapefil.h"

struct MapRegion
{
    QString name;
    QVector<QPolygonF> polygons;
    QString agsStr;
    int ags;
    int parts;
    int ewz;
    QString bez;
};

void readShapeFile(const QString &filename, QVector<MapRegion> &regions);

double getDirection(const QPolygonF &polygon);

#endif // SHAPELIBUTILS_H
