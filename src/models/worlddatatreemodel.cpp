#include "worlddatatreemodel.h"
#include "worldtreeitem.h"
#include "../data/world.h"

WorldDataTreeModel::WorldDataTreeModel(const world::World &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new WorldTreeItem({tr("Land / Provinz")});
    setupModelData(data, rootItem);
}

WorldDataTreeModel::~WorldDataTreeModel()
{
    delete rootItem;
}

QVariant WorldDataTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    WorldTreeItem *item = static_cast<WorldTreeItem*>(index.internalPointer());

    return item->data(index.column());
}

QVector<QVariant> WorldDataTreeModel::rowData(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVector<QVariant>(rootItem->columnCount());
    }

    if(role != Qt::DisplayRole) {
        return QVector<QVariant>(rootItem->columnCount());
    }

    WorldTreeItem *item = static_cast<WorldTreeItem*>(index.internalPointer());

    return item->rowData(index.column());
}

QPair<QString, QString> WorldDataTreeModel::key(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QPair<QString, QString>();
    }

    if(role != Qt::DisplayRole) {
        return QPair<QString, QString>();
    }

    WorldTreeItem *item = static_cast<WorldTreeItem*>(index.internalPointer());

    return item->key(index.column());
}

Qt::ItemFlags WorldDataTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QVariant WorldDataTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootItem->data(section);
    }

    return QVariant();
}

QModelIndex WorldDataTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    WorldTreeItem *parentItem;

    if(!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<WorldTreeItem*>(parent.internalPointer());
    }

    WorldTreeItem *childItem = parentItem->child(row);
    if(childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex WorldDataTreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return QModelIndex();
    }

    WorldTreeItem *childItem = static_cast<WorldTreeItem*>(index.internalPointer());
    WorldTreeItem *parentItem = childItem->parentItem();

    if(parentItem == rootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int WorldDataTreeModel::rowCount(const QModelIndex &parent) const
{
    WorldTreeItem *parentItem;
    if(parent.column() > 0) {
        return 0;
    }

    if(!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<WorldTreeItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
}

int WorldDataTreeModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return static_cast<WorldTreeItem*>(parent.internalPointer())->columnCount();
    }

    return rootItem->columnCount();
}

void WorldDataTreeModel::setupModelData(const world::World &modelData, WorldTreeItem *parent)
{
    // start with first element - the country
    WorldTreeItem *worldItem = new WorldTreeItem({QString("%0").arg(modelData.name)},
                                                 QPair<QString, QString>(modelData.name, ""),
                                                 parent);
    parent->appendChild(worldItem);
    // add countries
    for(const world::Country &country : modelData.countries) {
        WorldTreeItem *countryItem = new WorldTreeItem({QString("%0").arg(country.name)},
                                                       QPair<QString, QString>(country.name, ""),
                                                       worldItem);
        worldItem->appendChild(countryItem);

        // add districts
        for(const world::Province &province : country.provinces) {
            WorldTreeItem *provinceItem = new WorldTreeItem({province.name},
                                                            QPair<QString, QString>(country.name, province.name),
                                                            countryItem);
            countryItem->appendChild(provinceItem);
        }
    }
}

