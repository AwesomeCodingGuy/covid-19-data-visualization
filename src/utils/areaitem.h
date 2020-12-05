#ifndef AREAITEM_H
#define AREAITEM_H

#include <QGraphicsPathItem>
#include <QDateTime>

#include "graphicsitems.h"

class CovidDataTreeItem;

class AreaItem : public QGraphicsPathItem
{
public:
    enum { Type = UserType + CustomGraphicsTypes::AreaType};

    AreaItem(QGraphicsItem *parent = nullptr);
    AreaItem(const QPainterPath &path, QGraphicsItem *parent = nullptr);

    int type() const override;

    QString getName() const;
    void setName(const QString &newName);

    QString getDesignation() const;
    void setDesignation(const QString &newDesignation);

    void setCompleteName(const QString &newName, const QString &newDesignation);

    void setIncidence7(float value);
    float getIncidence7() const;

    void setLatestData(QDateTime time, int casesToday, int deathsToday,
                       int casesTotal, int deathsTotal, float incidence7);

    CovidDataTreeItem *getDataset() const;
    void setDataset(CovidDataTreeItem *value);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    void init();
    void setColor();
    QColor getColorFromIncidence(float value);
    void updateTooltip();

private:
    QString name;
    QString designation;

    QDateTime timestamp;
    float incidence7;
    int casesToday;
    int deathsToday;
    int casesTotal;
    int deathsTotal;

    CovidDataTreeItem *dataset;
};

#endif // AREAITEM_H
