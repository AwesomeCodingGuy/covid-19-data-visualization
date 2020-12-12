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
    class QDateTimeAxis;
    class QValueAxis;
    class QAbstractAxis;
}

class ChartView;
class CovidDataTreeItem;

class ChartWidget : public QWidget
{
    Q_OBJECT

    enum ChartType {
        Cumulated               = 0,
        Daily                   = 1,
        Acceleration            = 2,
        CumulatedCasesCompare   = 3,
        CumulatedDeathsCompare  = 4,
        DailyCasesCompare       = 5,
        DailyDeathsCompare      = 6,
        SevenDayCsesCompare     = 7,
    };

public:
    explicit ChartWidget(const QVector<QDateTime> &timestamps,
                         const CaseData &caseData,
                         QWidget *parent = nullptr);
    explicit ChartWidget(const CaseData &caseData,
                         QWidget *parent = nullptr);
    explicit ChartWidget(QVector<const CovidDataTreeItem *> caseDataItems,
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

    void addNewChartView(QtCharts::QChart *chart, ChartType type);

    void connectMarkers(const QtCharts::QChart &chart);
    int getOptimalTickinterval(int maxValue);
    int getOptimalTickinterval(int minValue, int maxValue);

    void initDateTimeVector(const QDate &start, const int values);
    void calculateTimestamps(const QVector<const CovidDataTreeItem *> caseDataItems);

    void initCumulatedSeries(const QVector<const CovidDataTreeItem *> caseDataItems);

    QtCharts::QDateTimeAxis* createDateTimeAxis();
    QtCharts::QValueAxis* createValueAxis(qreal min, qreal max);
    QtCharts::QChart* createChart(const QString &title,
                                  QtCharts::QAbstractAxis *xAxis,
                                  QtCharts::QAbstractAxis *yAxis);

private:
    // TODO: make this of type QDate
    QVector<QDateTime> timestamps;
    CaseData caseData;

    QComboBox *chartSwitchCombo;
    QStackedWidget *chartContainer;
    QToolBar *toolBar;

    QPen pen_1;
    QPen pen_2;
    QPen pen_3;
};

#endif // CHARTWIDGET_H
