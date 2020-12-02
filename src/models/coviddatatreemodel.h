#ifndef COVIDDATATREEMODEL_H
#define COVIDDATATREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeItem;

class CovidDataTreeItem;

class CovidDataTreeModel : public QAbstractItemModel
{
public:
    explicit CovidDataTreeModel(const CovidDataTreeItem &data, QObject *parent = nullptr);
    ~CovidDataTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    QVector<QVariant> rowData(const QModelIndex &index, int role) const;
    const CovidDataTreeItem* getCovidDataItem(const QModelIndex &index) const;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void setupModelData(const CovidDataTreeItem &data, TreeItem *parent);

    TreeItem *rootItem;
};

#endif // COVIDDATATREEMODEL_H
