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
    void initGraphicalStuff();

    void addCumulatedChart();
    void addDailyChart();

    int getOptimalTickinterval(int maxValue);

    QVector<QDateTime> timestamps;
    CaseData caseData;

    QComboBox *chartSwitchCombo;
    QStackedWidget *chartContainer;

    QtCharts::QChartView *cumulatedChartView;
    QtCharts::QChartView *dailyChartView;

    QtCharts::QChart *cumulatedChart;
    QtCharts::QChart *dailyChart;

    QPen pen_1;
    QPen pen_2;
    QPen pen_3;
};

#endif // CHARTWIDGET_H
