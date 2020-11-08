#include "usatreeitem.h"

UsaTreeItem::UsaTreeItem(const QVector<QVariant> &data,
                     usa::Uid uidData,
                     UsaTreeItem *parentItem)
    : itemData(data)
    , uidData(uidData)
    , parent(parentItem)
{

}

UsaTreeItem::~UsaTreeItem()
{
    qDeleteAll(children);
}

void UsaTreeItem::appendChild(UsaTreeItem *child)
{
    children.append(child);
}

UsaTreeItem* UsaTreeItem::child(int row)
{
    if(row < 0 || row >= children.size()) {
        return nullptr;
    }
    return children.at(row);
}

int UsaTreeItem::childCount() const
{
    return children.count();
}

int UsaTreeItem::columnCount() const
{
    return itemData.count();
}

QVariant UsaTreeItem::data(int column) const
{
    if(column < 0 || column >= itemData.size()) {
        return QVariant();
    }
    return itemData.at(column);
}

QVector<QVariant> UsaTreeItem::rowData(int column) const
{
    if(column < 0 || column >= itemData.size()) {
        return QVector<QVariant>(columnCount());
    }
    return itemData;
}

usa::Uid UsaTreeItem::uid(int column) const
{
    if(column < 0 || column >= itemData.size()) {
        return usa::Uid();
    }
    return uidData;
}

int UsaTreeItem::row() const
{
    if(parent) {
        return parent->children.indexOf(const_cast<UsaTreeItem*>(this));
    }
    return 0;
}

UsaTreeItem* UsaTreeItem::parentItem()
{
    return parent;
}
