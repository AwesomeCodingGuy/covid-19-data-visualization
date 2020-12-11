#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QObject>
#include <QtCharts/QChartView>

namespace QtCharts {
class QLineSeries;
}

class Callout;

class ChartView : public QtCharts::QChartView
{
    Q_OBJECT
public:
    explicit ChartView(QtCharts::QChart *chart, QWidget *parent = nullptr);

private:
    qreal getValueFromSeries(const QtCharts::QLineSeries* series, QDateTime timePoint);
    QString displayWithThousandSeparator(qreal number);

public slots:
    void setTooltip(QPointF point, bool state);

signals:

private:
    Callout *tooltip;
};

#endif // CHARTVIEW_H
