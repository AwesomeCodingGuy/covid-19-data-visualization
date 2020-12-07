#ifndef COVIDDATATREEITEM_H
#define COVIDDATATREEITEM_H

#include <QString>
#include <QMap>
#include <QPainterPath>

#include "casedata.h"

class AreaItem;
class QGraphicsTextItem;

class CovidDataTreeItem
{
public:
    CovidDataTreeItem(CovidDataTreeItem *parent = nullptr);
    explicit CovidDataTreeItem(const QString &itemName, CovidDataTreeItem *parent = nullptr);
    ~CovidDataTreeItem();

    void setItemNameAlt(const QString &name);
    QString getItemNameAlt() const;

    void setParent(CovidDataTreeItem *parent);
    CovidDataTreeItem *getParent() const;

    void setCaseData(const CaseData &data);
    CaseData getCaseData() const;
    CaseData& getCaseData();
    CaseData* getCaseDataPtr();
    const CaseData* getCaseDataPtr() const;

    void addChild(int key, CovidDataTreeItem &item);

    CovidDataTreeItem* getChild(int key);
    CovidDataTreeItem* getChild(const QString &itemName);

    QMap<int, CovidDataTreeItem>& getChildren();
    const QMap<int, CovidDataTreeItem>& getChildren() const;

    void createAreaItem(QPainterPath path);
    AreaItem* getAreaItem() const;
    void setAreaItem(AreaItem *item);

    QString getItemName() const;
    void setItemName(const QString &value);

    QString getItemDesignation() const;
    void setItemDesignation(const QString &value);

    QPointF getLocation() const;
    void setLocation(const QPointF &value);

    void createTextItem();
    QGraphicsTextItem* getTextItem() const;

private:
    QString itemNameAlt;
    QString itemName;
    QString itemDesignation;
    CaseData data;
    QPointF location;

    CovidDataTreeItem *parent;
    QMap<int, CovidDataTreeItem> children;

    // visual representation for map
    AreaItem *areaItem;
    QGraphicsTextItem *textItem;
};

QString getCompleteName(const CovidDataTreeItem *treeItem);

#endif // COVIDDATATREEITEM_H
