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
        SevenDayCasesCompare    = 7,
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

    void initSimpleSeries(const QVector<const CovidDataTreeItem *> caseDataItems,
                          const QString &title,
                          ChartType chartType);

    QtCharts::QDateTimeAxis* createDateTimeAxis();
    QtCharts::QValueAxis* createValueAxis(qreal min, qreal max);
    QtCharts::QChart* createChart(const QString &title,
                                  QtCharts::QAbstractAxis *xAxis,
                                  QtCharts::QAbstractAxis *yAxis);

    const CaseSeries<int>& getSeriesReferenceInt(const CaseData &data,
                                                 ChartType chartType);
    const CaseSeries<float>& getSeriesReferenceFloat(const CaseData &data,
                                                     ChartType chartType);

private:
    QVector<QDateTime> timestamps;
    CaseData caseData;

    QComboBox *chartSwitchCombo;
    QStackedWidget *chartContainer;
    QToolBar *toolBar;
};

#endif // CHARTWIDGET_H
