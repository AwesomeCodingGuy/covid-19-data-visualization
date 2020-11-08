#include "usadatatreemodel.h"
#include "usatreeitem.h"
#include "../data/usa.h"

UsaDataTreeModel::UsaDataTreeModel(const usa::Usa &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new UsaTreeItem({tr("Staat / County"), tr("UID")});
    setupModelData(data, rootItem);
}

UsaDataTreeModel::~UsaDataTreeModel()
{
    delete rootItem;
}

QVariant UsaDataTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    UsaTreeItem *item = static_cast<UsaTreeItem*>(index.internalPointer());

    return item->data(index.column());
}

QVector<QVariant> UsaDataTreeModel::rowData(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVector<QVariant>(rootItem->columnCount());
    }

    if(role != Qt::DisplayRole) {
        return QVector<QVariant>(rootItem->columnCount());
    }

    UsaTreeItem *item = static_cast<UsaTreeItem*>(index.internalPointer());

    return item->rowData(index.column());
}

usa::Uid UsaDataTreeModel::uid(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return usa::Uid();
    }

    if(role != Qt::DisplayRole) {
        return usa::Uid();
    }

    UsaTreeItem *item = static_cast<UsaTreeItem*>(index.internalPointer());

    return item->uid(index.column());
}

Qt::ItemFlags UsaDataTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QVariant UsaDataTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootItem->data(section);
    }

    return QVariant();
}

QModelIndex UsaDataTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    UsaTreeItem *parentItem;

    if(!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<UsaTreeItem*>(parent.internalPointer());
    }

    UsaTreeItem *childItem = parentItem->child(row);
    if(childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex UsaDataTreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return QModelIndex();
    }

    UsaTreeItem *childItem = static_cast<UsaTreeItem*>(index.internalPointer());
    UsaTreeItem *parentItem = childItem->parentItem();

    if(parentItem == rootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int UsaDataTreeModel::rowCount(const QModelIndex &parent) const
{
    UsaTreeItem *parentItem;
    if(parent.column() > 0) {
        return 0;
    }

    if(!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<UsaTreeItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
}

int UsaDataTreeModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return static_cast<UsaTreeItem*>(parent.internalPointer())->columnCount();
    }

    return rootItem->columnCount();
}

void UsaDataTreeModel::setupModelData(const usa::Usa &modelData, UsaTreeItem *parent)
{
    // start with the first element - usa
    UsaTreeItem *usaItem = new UsaTreeItem({QString(modelData.name), modelData.uid.uid},
                                           modelData.uid,
                                           parent);
    parent->appendChild(usaItem);

    // add states
    for(const usa::State &state : modelData.states) {
        UsaTreeItem *satateItem = new UsaTreeItem({QString(state.name), state.uid.uid},
                                                  state.uid,
                                                  usaItem);
        usaItem->appendChild(satateItem);

        // add counties
        for(const usa::County &county : state.counties) {
            UsaTreeItem *countyItem = new UsaTreeItem({QString(county.name), county.uid.uid},
                                                      county.uid,
                                                      satateItem);
            satateItem->appendChild(countyItem);
        }
    }
}
