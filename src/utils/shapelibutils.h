#ifndef SHAPELIBUTILS_H
#define SHAPELIBUTILS_H

#include <QDebug>
#include <QString>
#include <QVector>
#include <QPolygonF>

#include "shapefil.h"
#include "../data/mapregion.h"

bool readShapeFileData(const QString &filename, QVector<MapRegion> &regions, bool convertToPositive = false);
bool readDatabaseFileGermany(const QString &filename, QVector<MapRegion> &regions);
bool readDatabaseFileUsa(const QString &filename, QVector<MapRegion> &regions);

double getDirection(const QPolygonF &polygon);

#endif // SHAPELIBUTILS_H
