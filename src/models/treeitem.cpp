#include "treeitem.h"

TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parentItem)
    : itemData(data)
    , parent(parentItem)
{

}

TreeItem::~TreeItem()
{
    qDeleteAll(children);
}

void TreeItem::appendChild(TreeItem *child)
{
    children.append(child);
}

TreeItem *TreeItem::child(int row)
{
    if(row < 0 || row >= children.size()) {
        return nullptr;
    }
    return children.at(row);
}

int TreeItem::childCount() const
{
    return children.count();
}

int TreeItem::columnCount() const
{
    return itemData.count();
}

QVariant TreeItem::data(int column) const
{
    if(column < 0 || column >= itemData.size()) {
        return QVariant();
    }
    return itemData.at(column);
}

QVector<QVariant> TreeItem::rowData(int column) const
{
    if(column < 0 || column >= itemData.size()) {
        return QVector<QVariant>(columnCount());
    }
    return itemData;
}

int TreeItem::row() const
{
    if(parent) {
        return parent->children.indexOf(const_cast<TreeItem*>(this));
    }
    return 0;
}

TreeItem *TreeItem::parentItem()
{
    return parent;
}
