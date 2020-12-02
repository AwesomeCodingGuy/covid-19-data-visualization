#include "coviddatatreemodel.h"

#include "treeitem.h"
#include "../data/coviddatatreeitem.h"

CovidDataTreeModel::CovidDataTreeModel(const CovidDataTreeItem &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TreeItem({tr("Datensatz")}, data);

    if(data.getItemNameAlt() == "root") {
        for(const CovidDataTreeItem &childData : data.getChildren()) {
            setupModelData(childData, rootItem);
        }
    } else {
        setupModelData(data, rootItem);
    }
}

CovidDataTreeModel::~CovidDataTreeModel()
{
    delete rootItem;
}

QVariant CovidDataTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

QVector<QVariant> CovidDataTreeModel::rowData(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVector<QVariant>(rootItem->columnCount());
    }

    if(role != Qt::DisplayRole) {
        return QVector<QVariant>(rootItem->columnCount());
    }

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->rowData(index.column());
}

const CovidDataTreeItem *CovidDataTreeModel::getCovidDataItem(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return nullptr;
    }

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return &item->getCovidDataItem();
}

Qt::ItemFlags CovidDataTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QVariant CovidDataTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootItem->data(section);
    }

    return QVariant();
}

QModelIndex CovidDataTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    TreeItem *parentItem;

    if(!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    }

    TreeItem *childItem = parentItem->child(row);
    if(childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex CovidDataTreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return QModelIndex();
    }

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if(parentItem == rootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int CovidDataTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if(parent.column() > 0) {
        return 0;
    }

    if(!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
}

int CovidDataTreeModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    }

    return rootItem->columnCount();
}

void CovidDataTreeModel::setupModelData(const CovidDataTreeItem &data, TreeItem *parent)
{
    // add item
    TreeItem *newItem = new TreeItem({data.getItemNameAlt()}, data, parent);
    parent->appendChild(newItem);

    // add its children recursively to the item
    for(const CovidDataTreeItem &childData : data.getChildren()) {
        setupModelData(childData, newItem);
    }
}
