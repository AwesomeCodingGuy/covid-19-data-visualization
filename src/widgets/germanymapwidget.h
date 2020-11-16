#ifndef GERMANYMAPWIDGET_H
#define GERMANYMAPWIDGET_H

#include <QMap>
#include <QWidget>

#include "../utils/shapelibutils.h"

class MapView;
class QGraphicsScene;
class QGraphicsItem;
class AreaItem;

namespace germany {
class Germany;
}

class GermanyMapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GermanyMapWidget(QWidget *parent = nullptr);

    bool loadGermanMap(const QString &file);

    void setData(germany::Germany *germanyData);

private:
    QColor getColorFromIncidenceValue(float value);

public slots:
    void pathItemClicked(AreaItem *item);

signals:
    void newChartRequested(int);

private:
    static constexpr int agsItemDataIndex = 0;

    MapView *graphicsView;
    QGraphicsScene *scene;

    QVector<MapRegion> regions;
    QMap<int, QGraphicsItem*> shapeMap;

    germany::Germany *germanyData;
};

#endif // GERMANYMAPWIDGET_H
