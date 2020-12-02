#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QVector>

class CovidDataTreeItem;

class TreeItem
{
public:
    explicit TreeItem(const QVector<QVariant> &data, const CovidDataTreeItem &covidDataItem, TreeItem *parentItem = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    QVector<QVariant> rowData(int column) const;
    const CovidDataTreeItem& getCovidDataItem() const;
    int row() const;
    TreeItem *parentItem();

private:
    QVector<TreeItem*> children;
    QVector<QVariant> itemData;
    TreeItem *parent;
    const CovidDataTreeItem &covidDataItem;
};

#endif // TREEITEM_H

