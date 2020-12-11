#include "chartview.h"

#include "../utils/callout.h"

#include <QLocale>
#include <QDebug>
#include <QDateTime>
#include <QtCharts/QAbstractSeries>
#include <QtCharts/QLineSeries>

#include <algorithm>

ChartView::ChartView(QtCharts::QChart *chart, QWidget *parent)
    : QtCharts::QChartView(chart, parent)
    , tooltip(nullptr)
{

}

void ChartView::setTooltip(QPointF point, bool state)
{
    QtCharts::QAbstractSeries *series = qobject_cast<QtCharts::QAbstractSeries*>(sender());
    if(tooltip == 0) {
        tooltip = new Callout(chart(), series);
    }

    if(state) {
        if(series->type() == QtCharts::QAbstractSeries::SeriesTypeLine) {
            // Get value stored in seriesm since the mapping only approximates the mouse position.
            auto lineSeries = qobject_cast<QtCharts::QLineSeries*>(series);
            qreal yValue = getValueFromSeries(lineSeries, QDateTime::fromMSecsSinceEpoch(point.x()));

            // calculate the time string and set rich text for the tooltip
            QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(point.x());
            QString date = dateTime.date().toString("d. MMMM yy");
            QString text = QString(
                        "<table>"
                        " <tr>"
                        "  <td>Datum:</td>"
                        "  <td align=\"right\">%2</td>"
                        " </tr><tr>"
                        "  <td>%1:</td>"
                        "  <td align=\"right\">%3</td>"
                        " </tr>"
                        "</table>"
            ).arg(series->name())
            .arg(date)
            .arg(displayWithThousandSeparator(yValue));

            // set tooltip data.
            tooltip->setText(text);
            tooltip->setAnchor(point);
            tooltip->setZValue(11);
            tooltip->updateGeometry();
            tooltip->show();
        }
    } else {
        tooltip->hide();
    }
}

bool comparePointX(QPointF pt1, QPointF pt2)
{
    return pt1.x() < pt2.x();
}

qreal ChartView::getValueFromSeries(const QtCharts::QLineSeries* series, QDateTime timePoint)
{
    timePoint.setTime(QTime(0, 0, 0));

    const auto pts = series->points();
    QPointF pt = *std::lower_bound(pts.cbegin(), pts.cend(),
                                   QPointF(timePoint.toMSecsSinceEpoch(), 0), comparePointX);

    return pt.y();
}

QString ChartView::displayWithThousandSeparator(qreal number)
{
    QLocale german(QLocale::German, QLocale::Germany);
    return german.toString(number, 'f', 0);
}
