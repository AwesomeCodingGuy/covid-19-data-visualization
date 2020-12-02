#include "coviddatatreeitem.h"

// STL
#include <utility>

#include "../utils/areaitem.h"

CovidDataTreeItem::CovidDataTreeItem(CovidDataTreeItem *parent)
    : CovidDataTreeItem("", parent)
{

}

CovidDataTreeItem::CovidDataTreeItem(const QString &itemName, CovidDataTreeItem *parent)
    : itemNameAlt(itemName)
    , parent(parent)
    , areaItem(nullptr)
{

}

CovidDataTreeItem::~CovidDataTreeItem()
{

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
        incidence = 7.0f * data.casesSevenDayAverage.last() * 100000.f / static_cast<float>(data.population);
    }

    QDateTime dTime;
    dTime.setDate(data.startDate.addDays(data.cases.size() - 1));

    areaItem = new AreaItem(path);
    areaItem->setCompleteName(itemName, itemDesignation);
    areaItem->setLatestData(dTime,
                            data.cases.last(),
                            data.deaths.last(),
                            data.casesCumulated.last(),
                            data.deathsCumulated.last(),
                            incidence);
    areaItem->setDataset(this);
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
