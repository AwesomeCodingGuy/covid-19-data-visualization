#ifndef AREAITEM_H
#define AREAITEM_H

#include <QGraphicsPathItem>
#include <QDateTime>

#include "graphicsitems.h"

class AreaItem : public QGraphicsPathItem
{
    static constexpr QColor Neutral {180, 180, 180};
    static constexpr QColor Green   {112, 199, 120};
    static constexpr QColor Orange  {255, 171, 0};
    static constexpr QColor Red     {183, 55, 68};
    static constexpr QColor DarkRed {112, 39, 23};
    static constexpr QColor Black   {0, 0, 0};

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
};

#endif // AREAITEM_H
