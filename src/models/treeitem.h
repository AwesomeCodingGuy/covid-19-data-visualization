#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QVector>

class TreeItem
{
public:
    explicit TreeItem(const QVector<QVariant> &data, TreeItem *parentItem = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    QVector<QVariant> rowData(int column) const;
    int row() const;
    TreeItem *parentItem();

private:
    QVector<TreeItem*> children;
    QVector<QVariant> itemData;
    TreeItem *parent;
};

#endif // TREEITEM_H

