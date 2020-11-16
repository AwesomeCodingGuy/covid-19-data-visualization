#include "germanymapwidget.h"

#include "mapview.h"
#include "../data/casedata.h"
#include "../data/germany.h"
#include "../utils/shapelibutils.h"
#include "../utils/areaitem.h"

#include <functional>
#include <algorithm>

#include <QRandomGenerator>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QGraphicsPolygonItem>

GermanyMapWidget::GermanyMapWidget(QWidget *parent)
    : QWidget(parent)
    , germanyData(nullptr)
{
    scene = new QGraphicsScene();
    graphicsView = new MapView();
    graphicsView->setScene(scene);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(graphicsView);
    setLayout(layout);

    connect(graphicsView, &MapView::pathItemDoubleClicked,
            this, &GermanyMapWidget::pathItemClicked);

    loadGermanMap("data/Kreisgrenzen_2018_mit_Einwohnerzahl-shp/KRS_18_ew.shp");
}

bool GermanyMapWidget::loadGermanMap(const QString &file)
{
    // read shapefile
    readShapeFile(file, regions);

    for(MapRegion &region : regions) {
        QPainterPath path;
        // add polygons to painter path
        for(const QPolygonF &polygon : region.polygons) {
            path.addPolygon(polygon);
        }

        // create graphics path item
        AreaItem *item = new AreaItem(path);
        item->setData(agsItemDataIndex, region.ags);
        item->setName(region.name);

        // add item to scene and ags map
        scene->addItem(item);
        shapeMap.insert(region.ags, item);
    }

    scene->setSceneRect(scene->itemsBoundingRect());
    scene->update();

    graphicsView->setSceneRect(scene->itemsBoundingRect());

    graphicsView->update();

    graphicsView->scale(1, -1);

    return true;
}

void GermanyMapWidget::setData(germany::Germany *germanyData)
{
    this->germanyData = germanyData;

    for(MapRegion region : regions) {
        QGraphicsItem *item = shapeMap.value(region.ags);
        AreaItem *path = qgraphicsitem_cast<AreaItem*>(item);

        CaseData &caseData = germanyData->getDistrictByAgs(region.ags)->data;

        float incidence = 7.0f * caseData.casesSevenDayAverage.last() * 100000.f / static_cast<float>(region.ewz);
        path->setLatestData(germanyData->getTimestamps().last(),
                            caseData.cases.last(),
                            caseData.deaths.last(),
                            caseData.casesCumulated.last(),
                            caseData.deathsCumulated.last(),
                            incidence);
    }
}

void GermanyMapWidget::pathItemClicked(AreaItem *item)
{
    QVariant ags = item->data(agsItemDataIndex);
    if(!ags.isNull()) {
        newChartRequested(ags.toInt());
    }
}
