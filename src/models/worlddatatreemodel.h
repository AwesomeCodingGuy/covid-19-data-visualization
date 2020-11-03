#ifndef WORLDDATATREEMODEL_H
#define WORLDDATATREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class WorldTreeItem;
namespace world {
    class World;
}

class WorldDataTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit WorldDataTreeModel(const world::World &data, QObject *parent = nullptr);
    ~WorldDataTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    QVector<QVariant> rowData(const QModelIndex &index, int role) const;
    QPair<QString, QString> key(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void setupModelData(const world::World &modelData, WorldTreeItem *parent);

    WorldTreeItem *rootItem;
};

#endif // WORLDDATATREEMODEL_H
