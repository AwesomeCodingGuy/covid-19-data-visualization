#include "mapwidget.h"

#include "mapview.h"
#include "spoilerwidget.h"
#include "../data/coviddatatreeitem.h"
#include "../utils/areaitem.h"

#include <QDebug>
#include <QBoxLayout>
#include <QComboBox>

MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent)
{
    initUi();
}

MapWidget::~MapWidget()
{
    // delete all scenes
    for(auto i : scenes) {
        i->deleteLater();
    }
}

void MapWidget::initUi()
{
    mapView = new MapView();
    mapView->setBackgroundBrush(QBrush(QColor(101, 102, 203)));
    mapView->scale(1, -1);
    mapView->setDragMode(QGraphicsView::ScrollHandDrag);
    connect(mapView, &MapView::pathItemDoubleClicked,
            this, &MapWidget::pathItemDoubleClicked);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(mapView);
    setLayout(hLayout);

    // widgets without layout
    // Scene select combo
    sceneSelectCombo = new QComboBox(this);
    sceneSelectCombo->move(9, 9);
    connect(sceneSelectCombo, &QComboBox::currentTextChanged,
            this, &MapWidget::sceneSelectComboChanged);

    // color legend
    colorLegend = new SpoilerWidget(this);
    colorLegend->resize(400, 100);
    colorLegend->move(this->width()-400, 0);
}

void MapWidget::resetSceneMap()
{
    // delete all scenes in map
    qDeleteAll(scenes);

    // delete all items in combo box
    sceneSelectCombo->clear();
}

void MapWidget::sceneSelectComboChanged(const QString &text)
{
    auto scene = scenes.find(text);

    if(scene != scenes.end()) {
        mapView->setScene(*scene);
        mapView->fitInView((*scene)->sceneRect(), Qt::KeepAspectRatio);
    }
}

bool MapWidget::loadMapData(const CovidDataTreeItem &rootItem)
{
    resetSceneMap();

    // add a scene for every dataset item
    for(const CovidDataTreeItem &sceneItem : rootItem.getChildren()) {
        addScene(sceneItem);
    }

    // resize the combobox to largest item
    int width = sceneSelectCombo->minimumSizeHint().width();
    sceneSelectCombo->setMinimumWidth(width);

    // set first scene
    if(!scenes.empty()) {
        sceneSelectCombo->setCurrentIndex(0);
        sceneSelectComboChanged(sceneSelectCombo->currentText());
    }

    return true;
}

void MapWidget::addScene(const CovidDataTreeItem &sceneItem)
{
    // create new scene
    QGraphicsScene *newScene = new QGraphicsScene();

    // recursively add a graphics item for every map data item
    recursiveAddGraphicsItemsToScene(newScene, sceneItem);

    // do not add the scene if no items were added
    if(newScene->items().empty()) {
        qDebug() << "No Item in " << sceneItem.getItemNameAlt();
        delete newScene;
        return;
    }

    // add some padding to the scene rect
    adjustSceneRect(newScene, 200);

    // insert created scene in scene map
    scenes.insert(sceneItem.getItemNameAlt(), newScene);

    // insert in ComboBox for scene selection
    sceneSelectCombo->addItem(sceneItem.getItemNameAlt());
}

void MapWidget::recursiveAddGraphicsItemsToScene(QGraphicsScene *scene, const CovidDataTreeItem &sceneItem)
{
    // iterate over children
    for(const CovidDataTreeItem &sceneItem : sceneItem.getChildren()) {
        recursiveAddGraphicsItemsToScene(scene, sceneItem);
    }

    // if the path variable is not empty, a graphics item can be added to the scene
    if(sceneItem.getAreaItem() ) {
        scene->addItem(sceneItem.getAreaItem());
    }
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    // we need to manually move the legend to the right position
    colorLegend->move(this->width() - colorLegend->width() - 9, 9);
}

void MapWidget::adjustSceneRect(QGraphicsScene *scene, int value)
{
    auto bRect = scene->itemsBoundingRect();
    bRect.adjust(-value, -value, value, value);
    scene->setSceneRect(bRect);
}
