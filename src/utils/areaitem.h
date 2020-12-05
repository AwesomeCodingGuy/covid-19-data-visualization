#ifndef AREAITEM_H
#define AREAITEM_H

#include <QGraphicsPathItem>
#include <QDateTime>

#include "graphicsitems.h"

class CovidDataTreeItem;

class AreaItem : public QGraphicsPathItem
{
    static constexpr QColor Neutral     = QColor{180, 180, 180};
    static constexpr QColor Green       = QColor{112, 199, 120};
    static constexpr QColor LightGreen  = QColor{200, 200, 120};
    static constexpr QColor Yellow      = QColor{255, 200, 120};
    static constexpr QColor Orange      = QColor{255, 171, 0};
    static constexpr QColor LightRed    = QColor{238, 90, 24};
    static constexpr QColor Red         = QColor{183, 55, 68};
    static constexpr QColor DarkRed     = QColor{112, 39, 23};
    static constexpr QColor Magenta     = QColor{255, 0, 255};
    static constexpr QColor Black       = QColor{0, 0, 0};

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
