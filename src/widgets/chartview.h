#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QObject>
#include <QtCharts/QChartView>

class ChartView : public QtCharts::QChartView
{
    Q_OBJECT
public:
    explicit ChartView(QtCharts::QChart *chart, QWidget *parent = nullptr);

signals:

};

#endif // CHARTVIEW_H
