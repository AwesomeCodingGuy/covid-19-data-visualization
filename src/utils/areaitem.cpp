#include "areaitem.h"

#include <QLocale>
#include <QPen>
#include <QBrush>
#include <QObject>

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
    setPen(QPen(Qt::black));
    setBrush(QBrush(getColorFromIncidence(incidence7)));
}

QColor AreaItem::getColorFromIncidence(float value)
{
    if(value < 0.f) {
        return Neutral;
    } else if(value < 35.f) {
        return Green;
    } else if(value < 50.f) {
        return Orange;
    } else if(value < 100.f) {
        return Red;
    } else {
        return DarkRed;
    }
}

void AreaItem::updateTooltip()
{
    QString tip = QString(
                "<table style=\"font-size: large\">"
                " <tr>"
                "  <th style=\"border-bottom: 1px solid #000000;\"><b>%1</b></th>"
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
    ).arg(name)
     .arg(timestamp.toString("dd.MM.yyyy"))
     .arg(casesToday)
     .arg(deathsToday)
     .arg(casesTotal)
     .arg(deathsTotal)
     .arg(QLocale(QLocale::German).toString(incidence7, 'f', 1))
     .arg(getColorFromIncidence(incidence7).name());

    setToolTip(tip);
}
