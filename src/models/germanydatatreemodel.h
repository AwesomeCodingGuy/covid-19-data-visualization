#ifndef GERMANYDATATREEMODEL_H
#define GERMANYDATATREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeItem;
namespace germany {
    struct Country;
}

class GermanyDataTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit GermanyDataTreeModel(const germany::Country &data, QObject *parent = nullptr);
    ~GermanyDataTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    QVector<QVariant> rowData(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void setupModelData(const germany::Country &modelData, TreeItem *parent);

    TreeItem *rootItem;
};

#endif // GERMANYDATATREEMODEL_H
