#include "coviddatatreeitem.h"

// Qt
#include <QDebug>
#include <QGraphicsTextItem>

// STL
#include <limits>
#include <cmath>
#include <utility>

#include "colors.h"
#include "../utils/areaitem.h"

CovidDataTreeItem::CovidDataTreeItem(CovidDataTreeItem *parent)
    : CovidDataTreeItem("", parent)
{

}

CovidDataTreeItem::CovidDataTreeItem(const QString &itemName, CovidDataTreeItem *parent)
    : itemNameAlt(itemName)
    , location(std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity())
    , parent(parent)
    , areaItem(nullptr)
    , textItem(nullptr)
{

}

CovidDataTreeItem::~CovidDataTreeItem()
{
    if(areaItem && !areaItem->scene()) {
        delete areaItem;
    }
    if(textItem && !textItem->scene()) {
        delete textItem;
    }
}

void CovidDataTreeItem::setItemNameAlt(const QString &name)
{
    this->itemNameAlt = name;
}

QString CovidDataTreeItem::getItemNameAlt() const
{
    return itemNameAlt;
}

void CovidDataTreeItem::setParent(CovidDataTreeItem *parent)
{
    this->parent = parent;
}

CovidDataTreeItem* CovidDataTreeItem::getParent() const
{
    return parent;
}

void CovidDataTreeItem::setCaseData(const CaseData &data)
{
    this->data = data;
}

CaseData CovidDataTreeItem::getCaseData() const
{
    return data;
}

CaseData& CovidDataTreeItem::getCaseData()
{
    return data;
}

CaseData* CovidDataTreeItem::getCaseDataPtr()
{
    return &data;
}

const CaseData* CovidDataTreeItem::getCaseDataPtr() const
{
    return &data;
}

void CovidDataTreeItem::addChild(int key, CovidDataTreeItem &item)
{
    auto it = children.insert(key, item);
    it->setParent(this);
}

CovidDataTreeItem* CovidDataTreeItem::getChild(int key)
{
    QMap<int, CovidDataTreeItem>::iterator it = children.find(key);

    return (it != children.end())
                ? &(*it)
                : nullptr;
}

CovidDataTreeItem *CovidDataTreeItem::getChild(const QString &itemName)
{
    for(CovidDataTreeItem &item : children) {
        if(item.getItemNameAlt() == itemName) {
            return &item;
        }
    }

    return nullptr;
}

QMap<int, CovidDataTreeItem>& CovidDataTreeItem::getChildren()
{
    return children;
}

const QMap<int, CovidDataTreeItem> &CovidDataTreeItem::getChildren() const
{
    return children;
}

void CovidDataTreeItem::createAreaItem(QPainterPath path)
{
    float incidence = 0;
    if(data.population != 0) {
        incidence = 7.0f * data.casesSevenDayAverage.series.last() * 100000.f / static_cast<float>(data.population);
    }

    QDateTime dTime;
    dTime.setDate(data.startDate.addDays(data.cases.series.size() - 1));

    areaItem = new AreaItem(path);
    areaItem->setCompleteName(itemName, itemDesignation);
    areaItem->setLatestData(dTime,
                            data.cases.series.last(),
                            data.deaths.series.last(),
                            data.casesCumulated.series.last(),
                            data.deathsCumulated.series.last(),
                            incidence);
    areaItem->setDataset(this);

    createTextItem();
}

AreaItem* CovidDataTreeItem::getAreaItem() const
{
    return areaItem;
}

void CovidDataTreeItem::setAreaItem(AreaItem *item)
{
    areaItem = item;
}

QString CovidDataTreeItem::getItemName() const
{
    return itemName;
}

void CovidDataTreeItem::setItemName(const QString &value)
{
    itemName = value;
}

QString CovidDataTreeItem::getItemDesignation() const
{
    return itemDesignation;
}

void CovidDataTreeItem::setItemDesignation(const QString &value)
{
    itemDesignation = value;
}

QPointF CovidDataTreeItem::getLocation() const
{
    return location;
}

void CovidDataTreeItem::setLocation(const QPointF &value)
{
    location = value;
}

void CovidDataTreeItem::createTextItem()
{
    if(std::isinf(location.x()) || std::isinf(location.y())) {
        return;
    }

    textItem = new QGraphicsTextItem(itemNameAlt);

    textItem->setDefaultTextColor(colors::MapText);
    textItem->setPos(location.x() - (textItem->boundingRect().width() / 2) * .4f,
                     location.y() - (textItem->boundingRect().height() / 2) * -.4f);
    textItem->setZValue(1);
    textItem->setTransform(QTransform::fromScale(.4f, -.4f));
    textItem->setAcceptHoverEvents(false);
}

QGraphicsTextItem *CovidDataTreeItem::getTextItem() const
{
    return textItem;
}

QString appendParentName(QString name, const CovidDataTreeItem *treeItem)
{
    if(!treeItem || (treeItem->getItemNameAlt() == "root")) {
        return name;
    } else {
        name += ", " + treeItem->getItemNameAlt();
        return appendParentName(name, treeItem->getParent());
    }
}

QString getCompleteName(const CovidDataTreeItem *treeItem)
{
    QString name = treeItem->getItemNameAlt();

    return appendParentName(name, treeItem->getParent());
}
