#include "worldtreeitem.h"

WorldTreeItem::WorldTreeItem(const QVector<QVariant> &data,
                             QPair<QString, QString> keyData,
                             WorldTreeItem *parentItem)
    : itemData(data)
    , keyData(keyData)
    , parent(parentItem)
{

}

WorldTreeItem::~WorldTreeItem()
{
    qDeleteAll(children);
}

void WorldTreeItem::appendChild(WorldTreeItem *child)
{
    children.append(child);
}


WorldTreeItem* WorldTreeItem::child(int row)
{
    if(row < 0 || row >= children.size()) {
        return nullptr;
    }
    return children.at(row);
}

int WorldTreeItem::childCount() const
{
    return children.count();
}

int WorldTreeItem::columnCount() const
{
    return itemData.count();
}

QVariant WorldTreeItem::data(int column) const
{
    if(column < 0 || column >= itemData.size()) {
        return QVariant();
    }
    return itemData.at(column);
}

QVector<QVariant> WorldTreeItem::rowData(int column) const
{
    if(column < 0 || column >= itemData.size()) {
        return QVector<QVariant>(columnCount());
    }
    return itemData;
}

QPair<QString, QString> WorldTreeItem::key(int column) const
{
    if(column < 0 || column >= itemData.size()) {
        return QPair<QString, QString>();
    }
    return keyData;
}

int WorldTreeItem::row() const
{
    if(parent) {
        return parent->children.indexOf(const_cast<WorldTreeItem*>(this));
    }
    return 0;
}

WorldTreeItem* WorldTreeItem::parentItem()
{
    return parent;
}

