#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QVector>
#include <QDateTime>
#include <QDate>
#include <QWidget>
#include <QPen>

#include "../data/casedata.h"

class QComboBox;
class QStackedWidget;
class QPen;
class QToolBar;

namespace QtCharts {
    class QChart;
}

class ChartView;

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
    explicit ChartWidget(const CaseData &caseData,
                         QWidget *parent = nullptr);
    ~ChartWidget();

signals:

public slots:
    void resetZoom();
    void handleMarkerClicked();
    void rescale();

private:
    void initUi();
    void initCumulatedChart();
    void initDailyChart();
    void initAccelerationChart();

    void addNewChartView(ChartView *view, QtCharts::QChart *chart, ChartType type);

    void connectMarkers(const QtCharts::QChart &chart);
    int getOptimalTickinterval(int maxValue);
    int getOptimalTickinterval(int minValue, int maxValue);

private:
    // TODO: make this of type QDate
    QVector<QDateTime> timestamps;
    CaseData caseData;

    QComboBox *chartSwitchCombo;
    QStackedWidget *chartContainer;
    QToolBar *toolBar;

    ChartView *cumulatedChartView;
    ChartView *dailyChartView;
    ChartView *accelerationChartView;

    QtCharts::QChart *cumulatedChart;
    QtCharts::QChart *dailyChart;
    QtCharts::QChart *accelerationChart;

    QPen pen_1;
    QPen pen_2;
    QPen pen_3;
};

#endif // CHARTWIDGET_H
