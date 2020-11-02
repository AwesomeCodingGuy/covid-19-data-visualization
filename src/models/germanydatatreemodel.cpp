#include "germanydatatreemodel.h"
#include "treeitem.h"
#include "../data/germany.h"

GermanyDataTreeModel::GermanyDataTreeModel(const germany::Country &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TreeItem({"Name", "Code", "AGS"});
    setupModelData(data, rootItem);
}

GermanyDataTreeModel::~GermanyDataTreeModel()
{
    delete rootItem;
}

QVariant GermanyDataTreeModel::data(const QModelIndex &index, int role) const
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

QVector<QVariant> GermanyDataTreeModel::rowData(const QModelIndex &index, int role) const
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

Qt::ItemFlags GermanyDataTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QVariant GermanyDataTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootItem->data(section);
    }

    return QVariant();
}

QModelIndex GermanyDataTreeModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex GermanyDataTreeModel::parent(const QModelIndex &index) const
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

int GermanyDataTreeModel::rowCount(const QModelIndex &parent) const
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

int GermanyDataTreeModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    }

    return rootItem->columnCount();
}

void GermanyDataTreeModel::setupModelData(const germany::Country &modelData, TreeItem *parent)
{
    // start with first element - the country
    TreeItem *countryItem = new TreeItem({QString("%0 (%1)").arg(modelData.name).arg(modelData.states.size()),
                                          modelData.code,
                                          QVariant()},
                                         parent);
    parent->appendChild(countryItem);

    // add states
    for(const germany::State &state : modelData.states) {
        TreeItem *stateItem = new TreeItem({QString("%0 (%1)").arg(state.name).arg(state.districts.size()),
                                            state.code,
                                            QVariant()},
                                           countryItem);
        countryItem->appendChild(stateItem);

        // add districts
        for(const germany::District &district : state.districts) {
            TreeItem *districtItem = new TreeItem({QString("%0").arg(district.name),
                                                   QVariant(),
                                                   district.ags},
                                                  stateItem);
            stateItem->appendChild(districtItem);
        }
    }
}
