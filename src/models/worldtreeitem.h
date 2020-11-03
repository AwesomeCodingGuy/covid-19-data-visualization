#ifndef WORLDTREEITEM_H
#define WORLDTREEITEM_H

#include <QVariant>
#include <QVector>
#include <QPair>

class WorldTreeItem
{
public:
    explicit WorldTreeItem(const QVector<QVariant> &data,
                           QPair<QString, QString> key = QPair<QString, QString>(),
                           WorldTreeItem *parentItem = nullptr);
    ~WorldTreeItem();

    void appendChild(WorldTreeItem *child);

    WorldTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    QVector<QVariant> rowData(int column) const;
    QPair<QString, QString> key(int column) const;
    int row() const;
    WorldTreeItem *parentItem();

private:
    QVector<WorldTreeItem*> children;
    QVector<QVariant> itemData;
    QPair<QString, QString> keyData;
    WorldTreeItem *parent;
};

#endif // WORLDTREEITEM_H
