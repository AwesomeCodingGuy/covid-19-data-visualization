#ifndef USATREEITEM_H
#define USATREEITEM_H

#include <QVariant>
#include <QVector>

#include "../data/usa.h"

class UsaTreeItem
{
public:
    explicit UsaTreeItem(const QVector<QVariant> &data,
                         usa::Uid uidData = usa::Uid(),
                         UsaTreeItem *parentItem = nullptr);
    ~UsaTreeItem();

    void appendChild(UsaTreeItem *child);

    UsaTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    QVector<QVariant> rowData(int column) const;
    usa::Uid uid(int column) const;
    int row() const;
    UsaTreeItem *parentItem();

private:
    QVector<UsaTreeItem*> children;
    QVector<QVariant> itemData;
    usa::Uid uidData;
    UsaTreeItem *parent;
};

#endif // USATREEITEM_H
