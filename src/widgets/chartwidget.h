#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QVector>
#include <QDateTime>
#include <QWidget>
#include <QPen>

#include "../data/casedata.h"

class QComboBox;
class QStackedWidget;
class QPen;

namespace QtCharts {
    class QChart;
    class QChartView;
}

class ChartWidget : public QWidget
{
    Q_OBJECT

    enum ChartType {
        Cumulated       = 0,
        Daily           = 1,
        Acceleration    = 2
    };

public:
    explicit ChartWidget(const QVector<QDateTime> &timestamps,
                         const CaseData &caseData,
                         QWidget *parent = nullptr);
    ~ChartWidget();

signals:

private:
    void initUi();
    void initCumulatedChart();
    void initDailyChart();
    void initAccelerationChart();
    void initGraphicalStuff();

    void addCumulatedChart();
    void addDailyChart();
    void addAccelerationChart();

    int getOptimalTickinterval(int maxValue);

    QVector<QDateTime> timestamps;
    CaseData caseData;

    QComboBox *chartSwitchCombo;
    QStackedWidget *chartContainer;

    QtCharts::QChartView *cumulatedChartView;
    QtCharts::QChartView *dailyChartView;
    QtCharts::QChartView *accelerationChartView;

    QtCharts::QChart *cumulatedChart;
    QtCharts::QChart *dailyChart;
    QtCharts::QChart *accelerationChart;

    QPen pen_1;
    QPen pen_2;
    QPen pen_3;
};

#endif // CHARTWIDGET_H
