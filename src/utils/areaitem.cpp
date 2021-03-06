#include "areaitem.h"

#include <QLocale>
#include <QPen>
#include <QBrush>
#include <QObject>

#include "../data/colors.h"

AreaItem::AreaItem(QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , name("")
    , timestamp(QDateTime())
    , incidence7(-1.0f)
    , casesToday(0)
    , deathsToday(0)
    , casesTotal(0)
    , deathsTotal(0)
{
    init();
}

AreaItem::AreaItem(const QPainterPath &path, QGraphicsItem *parent)
    : QGraphicsPathItem(path, parent)
    , name("")
    , timestamp(QDateTime())
    , incidence7(-1.0f)
    , casesToday(0)
    , deathsToday(0)
    , casesTotal(0)
    , deathsTotal(0)
{
    init();
}

int AreaItem::type() const
{
    return Type;
}

QString AreaItem::getName() const
{
    return name;
}

void AreaItem::setName(const QString &newName)
{
    name = newName;

    updateTooltip();
}

QString AreaItem::getDesignation() const
{
    return designation;
}

void AreaItem::setDesignation(const QString &newDesignation)
{
    designation = newDesignation;

    updateTooltip();
}

void AreaItem::setCompleteName(const QString &newName, const QString &newDesignation)
{
    name = newName;
    designation = newDesignation;

    updateTooltip();
}

void AreaItem::setIncidence7(float value)
{
    incidence7 = value;

    setColor();
    updateTooltip();
}

float AreaItem::getIncidence7() const
{
    return incidence7;
}

void AreaItem::setLatestData(QDateTime time, int casesToday, int deathsToday,
                             int casesTotal, int deathsTotal, float incidence7)
{
    this->timestamp     = time;
    this->casesToday    = casesToday;
    this->deathsToday   = deathsToday;
    this->casesTotal    = casesTotal;
    this->deathsTotal   = deathsTotal;
    this->incidence7    = incidence7;

    setColor();
    updateTooltip();
}

void AreaItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setBrush(QBrush(brush().color().lighter()));

    QGraphicsPathItem::hoverEnterEvent(event);
}

void AreaItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setBrush(QBrush(getColorFromIncidence(incidence7)));

    QGraphicsPathItem::hoverLeaveEvent(event);
}

void AreaItem::init()
{
    setAcceptHoverEvents(true);
    setColor();
}

void AreaItem::setColor()
{
    setPen(QPen(colors::DarkGray));
    setBrush(QBrush(getColorFromIncidence(incidence7)));
}

QColor AreaItem::getColorFromIncidence(float value)
{
    for(const colors::ColorRange &colorRange : colors::ColorRanges) {
        if(colorRange.min <= value && value < colorRange.max) {
            return colorRange.color;
        }
    }

    return colors::Neutral;
}

void AreaItem::updateTooltip()
{
    QString tip = QString(
                "<table style=\"font-size: large\">"
                " <tr>"
                "  <th style=\"border-bottom: 1px solid #000000;\"><b>(%9)  %1</b></th>"
                "  <th style=\"border-bottom: 1px solid #000000;\" align=\"right\"><b>%2</b></th>"
                " </tr><tr>"
                "  <td>" + QObject::tr("Heute gemeldete Fälle") + "</td>"
                "  <td align=\"right\" style=\"padding-left: 30px;\">%3</td>"
                " </tr><tr>"
                "  <td>" + QObject::tr("Heute gemeldete Todesfälle") + "</td>"
                "  <td align=\"right\">%4</td>"
                " </tr><tr>"
                "  <td>" + QObject::tr("Fälle insgesamt") + "</td>"
                "  <td align=\"right\">%5</td>"
                " </tr><tr>"
                "  <td>" + QObject::tr("Todesfälle insgesamt") + "</td>"
                "  <td align=\"right\">%6</td>"
                " </tr><tr>"
                "  <td>" + QObject::tr("7-Tage-Inzidenz") + "</td>"
                "  <td align=\"right\" style=\"color: %8\">%7</td>"
                " </tr><tr>"
                "</table>"
    ).arg(name,
          timestamp.toString("dd.MM.yyyy"))
     .arg(casesToday)
     .arg(deathsToday)
     .arg(casesTotal)
     .arg(deathsTotal)
     .arg(QLocale(QLocale::German).toString(incidence7, 'f', 1),
          getColorFromIncidence(incidence7).name(),
          designation);

    setToolTip(tip);
}

CovidDataTreeItem *AreaItem::getDataset() const
{
    return dataset;
}

void AreaItem::setDataset(CovidDataTreeItem *value)
{
    dataset = value;
}
