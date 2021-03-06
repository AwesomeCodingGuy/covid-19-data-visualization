#include "mapwidget.h"

#include "mapview.h"
#include "spoilerwidget.h"
#include "colorlegend.h"
#include "../data/colors.h"
#include "../data/coviddatatreeitem.h"
#include "../utils/areaitem.h"

#include <QEvent>
#include <QScrollBar>
#include <QDebug>
#include <QBoxLayout>
#include <QComboBox>

constexpr Q_DECL_UNUSED int graphicsViewSpacing     = 9;
constexpr Q_DECL_UNUSED int initialScrollBarWidth   = 17;
constexpr Q_DECL_UNUSED int extraSpacing            = 2;

MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent)
{
    initUi();
}

MapWidget::~MapWidget()
{
    // delete all scenes
    for(auto i : qAsConst(scenes)) {
        i->deleteLater();
    }
}

void MapWidget::initUi()
{
    mapView = new MapView();
    mapView->setBackgroundBrush(QBrush(colors::MapBg));
    mapView->scale(1, -1);
    mapView->setDragMode(QGraphicsView::ScrollHandDrag);
    connect(mapView, &MapView::pathItemDoubleClicked,
            this, &MapWidget::pathItemDoubleClicked);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(mapView);
    hLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(hLayout);

    // widgets without layout
    // Scene select combo
    sceneSelectCombo = new QComboBox(this);
    sceneSelectCombo->move(extraSpacing,
                           extraSpacing);
    connect(sceneSelectCombo, &QComboBox::currentTextChanged,
            this, &MapWidget::sceneSelectComboChanged);

    // color legend
    QHBoxLayout *legendLayout = new QHBoxLayout();
    ColorLegend *legend = new ColorLegend();
    legendLayout->addWidget(legend);
    legendLayout->setContentsMargins(0, 0, 0, 0);

    colorLegend = new SpoilerWidget(tr("Farblegende"), 300, this);
    colorLegend->setContentLayout(*legendLayout);
    colorLegend->setCollapsed(false);
}

void MapWidget::resetSceneMap()
{
    // delete all scenes in map
    qDeleteAll(scenes);

    // delete all items in combo box
    sceneSelectCombo->clear();
}

void MapWidget::retranslateUi()
{
    colorLegend->setButtonName(tr("Farblegende"));
    colorLegend->updateGeometry();
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
    if(sceneItem.getAreaItem()) {
        scene->addItem(sceneItem.getAreaItem());
        if(sceneItem.getTextItem()) {
            scene->addItem(sceneItem.getTextItem());
        }
    }
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    // we need to manually move the legend to the right position
    int scrollBarWidth = initialScrollBarWidth;
    if(mapView->verticalScrollBar()->isVisible()) {
        scrollBarWidth = mapView->verticalScrollBar()->width();
    }

    colorLegend->move(this->width() - colorLegend->width() - scrollBarWidth - extraSpacing,
                      extraSpacing);
}

void MapWidget::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

void MapWidget::adjustSceneRect(QGraphicsScene *scene, int value)
{
    auto bRect = scene->itemsBoundingRect();
    bRect.adjust(-value, -value, value, value);
    scene->setSceneRect(bRect);
}
