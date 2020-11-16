#include "shapelibutils.h"

#include <QFile>

void readShapeFile(const QString &filename, QVector<MapRegion> &regions)
{
    try {
        SHPHandle handle = SHPOpen(filename.toStdString().c_str(), "rb");
        if(!handle) {
            throw QString("File %1 not found").arg(filename);
        }

        int shapeType;
        int entities;
        double adfMinBound[4];
        double adfMaxBound[4];

        SHPGetInfo(handle, &entities, &shapeType, adfMinBound, adfMaxBound);

        regions.resize(entities);

//        qDebug() << "Entities: " << entities;

        for(int i = 0; i < entities; ++i) {
            SHPObject *shape = SHPReadObject(handle, i);

            int id = shape->nShapeId;
            regions[id].parts = shape->nParts;

            // the data is scaled by 1000 for displaying reasons
            double factor = 100.0;

//            qDebug() << "*** ID: " << i << " Parts: " << shape->nParts;

            if(shape->nSHPType == SHPT_POLYGON) {
                if(regions[id].parts > 1) {
                    // init start indices
                    QVector<int> startIndices(regions[id].parts + 1);
                    for(int part = 0; part < regions[id].parts; ++part) {
                        startIndices[part] = shape->panPartStart[part];
                    }
                    startIndices[regions[id].parts] = shape->nVertices;

                    // add parts
                    for(int part = 0; part < regions[id].parts; ++part) {
                        QPolygonF polygon;
                        for(int vertex = startIndices[part]; vertex < startIndices[part +1]; ++vertex) {
                            polygon << QPointF(shape->padfX[vertex]*factor,
                                               shape->padfY[vertex]*factor);
                        }
                        polygon << QPointF(shape->padfX[startIndices[part]]*factor,
                                           shape->padfY[startIndices[part]]*factor);

//                        qDebug() << "     Direction: " << (getDirection(polygon) < 0 ? "CCW" : "CW")
//                                 << "    PartType: " << shape->panPartType[part];

                        regions[id].polygons.push_back(polygon);
                    }
                } else {
                    QPolygonF polygon;
                    for(int v = 0; v < shape->nVertices; ++v) {
                        polygon << QPointF(shape->padfX[v]*factor,
                                           shape->padfY[v]*factor);
                    }
//                    qDebug() << "     Direction: " << (getDirection(polygon) < 0 ? "CCW" : "CW");
                    regions[id].polygons.push_back(polygon);
                }
            }

            SHPDestroyObject(shape);
        }
        SHPClose(handle);

    } catch(const QString &s) {
        qDebug() << s;
    } catch(...) {
        qDebug() << "Other exception";
    }


    try {
        DBFHandle handle = DBFOpen(filename.toStdString().c_str(), "rb");
        if(!handle) {
            throw QString("File %1 not found").arg(filename);
        }

        int fields = DBFGetFieldCount(handle);
        int records = DBFGetRecordCount(handle);

        QVector<std::string> fieldNames(fields);
        QVector<int> fieldWidths(fields);
        QVector<int> fieldDecimals(fields);

        for(int i = 0; i < records; ++i) {
            regions[i].name     = DBFReadStringAttribute(handle, i, 5);
            regions[i].agsStr   = DBFReadStringAttribute(handle, i, 3);
            regions[i].ags      = regions[i].agsStr.toInt();
            regions[i].ewz      = DBFReadIntegerAttribute(handle, i, 18);
            regions[i].bez      = DBFReadStringAttribute(handle, i, 6);
        }

        DBFClose(handle);

    } catch(const QString &s) {
        qDebug() << s;
    } catch(...) {

    }
}

double getDirection(const QPolygonF &polygon)
{
    double sum = 0.0;
    for(int i = 0; i < polygon.size() - 1; ++i) {
        sum += (polygon[i + 1].x() - polygon[i].x()) * (polygon[i + 1].y() + polygon[i].y());
    }
    sum += (polygon[0].x() - polygon[polygon.size() - 1].x()) * (polygon[0].y() + polygon[polygon.size() - 1].y());

    return sum;
}
