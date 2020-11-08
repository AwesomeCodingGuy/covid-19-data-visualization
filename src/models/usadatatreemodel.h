#ifndef USADATATREEMODEL_H
#define USADATATREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "../data/usa.h"

class UsaTreeItem;

class UsaDataTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit UsaDataTreeModel(const usa::Usa &data, QObject *parent = nullptr);
    ~UsaDataTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    QVector<QVariant> rowData(const QModelIndex &index, int role) const;
    usa::Uid uid(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void setupModelData(const usa::Usa &modelData, UsaTreeItem *parent);

    UsaTreeItem *rootItem;
};

#endif // USADATATREEMODEL_H
