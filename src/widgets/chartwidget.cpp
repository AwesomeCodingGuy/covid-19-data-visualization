#include "chartwidget.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QBrush>
#include <QPen>
#include <QDebug>
#include <QToolBar>
#include <QStackedWidget>
#include <QComboBox>
#include <QBoxLayout>
#include <QIcon>

#include <QtCharts/QLegend>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegendMarker>

#include "chartview.h"
#include "../data/colors.h"
#include "../data/coviddatatreeitem.h"

ChartWidget::ChartWidget(const QVector<QDateTime> &timestamps,
                         const CaseData &caseData,
                         QWidget *parent)
    : QWidget(parent)
    , timestamps(timestamps)
    , caseData(caseData)
{
    // init UI
    initUi();

    // init charts
    initCumulatedChart();
    initDailyChart();
    initAccelerationChart();
}

ChartWidget::ChartWidget(const CaseData &caseData, QWidget *parent)
    : QWidget(parent)
    , caseData(caseData)
{
    // calculate timestamps
    initDateTimeVector(caseData.startDate, caseData.cases.series.size());

    // init UI
    initUi();

    // init charts
    initCumulatedChart();
    initDailyChart();
    initAccelerationChart();
}

ChartWidget::ChartWidget(QVector<const CovidDataTreeItem *> caseDataItems,
                         QWidget *parent)
    : QWidget(parent)
{
    // init UI
    initUi();

    // calculate timestamps
    calculateTimestamps(caseDataItems);

    // add series
    initSimpleSeries(caseDataItems, tr("Erkrankungen kumuliert"), ChartType::CumulatedCasesCompare);
    initSimpleSeries(caseDataItems, tr("Todesfälle kumuliert"), ChartType::CumulatedDeathsCompare);
    initSimpleSeries(caseDataItems, tr("Tägliche Neuerkrankungen"), ChartType::DailyCasesCompare);
    initSimpleSeries(caseDataItems, tr("Tägliche Todesfälle"), ChartType::DailyDeathsCompare);
    initSimpleSeries(caseDataItems, tr("7-Tage-Inzidenz"), ChartType::SevenDayCasesCompare);
}

ChartWidget::~ChartWidget()
{

}

void ChartWidget::resetZoom()
{
    QtCharts::QChartView *v = dynamic_cast<QtCharts::QChartView*>(chartContainer->widget(chartContainer->currentIndex()));
    if(v) {
        v->chart()->zoomReset();
    }
}

void ChartWidget::handleMarkerClicked()
{
    QtCharts::QLegendMarker* marker = qobject_cast<QtCharts::QLegendMarker*>(sender());

    if(!marker) {
        return;
    }

    switch(marker->type()) {
        case QtCharts::QLegendMarker::LegendMarkerType::LegendMarkerTypeXY:
        {
            // toggle visibility of series
            marker->series()->setVisible(!marker->series()->isVisible());

            // turn legend marker back to visible, since hiding series also hides the marker
            // and we don't want it to happen now.
            marker->setVisible(true);

            // Dim the marker, if series is not visible
            qreal alpha = 1.0;

            if(!marker->series()->isVisible())
                alpha = 0.5;

            QColor color;
            QBrush brush = marker->labelBrush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setLabelBrush(brush);

            brush = marker->brush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setBrush(brush);

            QPen pen = marker->pen();
            color = pen.color();
            color.setAlphaF(alpha);
            pen.setColor(color);
            marker->setPen(pen);

            break;
        }
        default:
            break;
    }
}

void ChartWidget::rescale()
{
    // TODO: There are still some problems with lags...however the application does not freeze at the moment

    // get current visible QChartView
    const QtCharts::QChartView *cView = qobject_cast<QtCharts::QChartView*>(chartContainer->widget(chartContainer->currentIndex()));
    if(!cView) {
        return;
    }

    // get current visible QChart
    const QtCharts::QChart *currentChart = cView->chart();

    //get min /max values of visible charts
    int min = 0;
    int max = 0;
    bool anyChartVisible = false;
    const QList<QtCharts::QAbstractSeries*> series = currentChart->series();
    foreach(QtCharts::QAbstractSeries *s, series) {
        if(s->isVisible()) {
            min = std::min(min, s->property("min").toInt());
            max = std::max(max, s->property("max").toInt());
            anyChartVisible = true;
        }
    }

    // break if there is no chart visible - there is no need for rescaling
    if(!anyChartVisible) {
        return;
    }

    // get vertical axes of chart
    const QList<QtCharts::QAbstractAxis*> axes = currentChart->axes(Qt::Orientation::Vertical);
    // cast first axis to QValueAxis - it is assumes that there is only 1
    auto verticalValueAxis = qobject_cast<QtCharts::QValueAxis*>(axes.first());

    // apply nice numbers to axis if it is a valid axis
    if(verticalValueAxis) {
        verticalValueAxis->setRange(1.1f * min, 1.1f * max);
        qDebug() << min << " - " << max << " Int: " << getOptimalTickinterval(min, max);
        verticalValueAxis->setTickInterval(getOptimalTickinterval(min, max));
    }
}

void ChartWidget::initUi()
{
    // set stacked widget as main content widget
    chartContainer = new QStackedWidget();

    // add a combo box to the widget above the stacked widget.
    chartSwitchCombo = new QComboBox();
    chartSwitchCombo->move(5, 5);
    chartSwitchCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(chartSwitchCombo, SIGNAL(currentIndexChanged(int)),
            chartContainer, SLOT(setCurrentIndex(int)));

    // cerate toolbar
    toolBar = new QToolBar();
    toolBar->addWidget(chartSwitchCombo);
    QAction *a = toolBar->addAction(QIcon(":/icons/images/icons/resetZoom_32x32.png"),
                                    tr("Zoom zurücksetzen"));
    connect(a, &QAction::triggered,
            this, &ChartWidget::resetZoom);

    // init layout
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMenuBar(toolBar);
    hLayout->addWidget(chartContainer);

    setLayout(hLayout);
}

void ChartWidget::initCumulatedChart()
{
    // init series
    QtCharts::QLineSeries *sCases = new QtCharts::QLineSeries();
    sCases->setPen(QPen(colors::ChartColors[2], 2));
    sCases->setName(tr("Erkrankungen"));
    QtCharts::QLineSeries *sDeaths = new QtCharts::QLineSeries();
    sDeaths->setPen(QPen(colors::ChartColors[0], 2));
    sDeaths->setName(tr("Todesfälle"));

    // init series data
    for(int i = 0; i < timestamps.size(); ++i) {
        sCases->append(timestamps[i].toMSecsSinceEpoch(), caseData.casesCumulated.series[i]);
        sDeaths->append(timestamps[i].toMSecsSinceEpoch(), caseData.deathsCumulated.series[i]);
    }

    sCases->setProperty("min", caseData.casesCumulated.min);
    sCases->setProperty("max", caseData.casesCumulated.max);
    sDeaths->setProperty("min", caseData.deathsCumulated.min);
    sDeaths->setProperty("max", caseData.deathsCumulated.max);

    // create chart elements
    QtCharts::QDateTimeAxis *axisX = createDateTimeAxis();
    QtCharts::QValueAxis *axisY = createValueAxis(1.1f * std::min(caseData.casesCumulated.min, caseData.deathsCumulated.min),
                                                  1.1f * std::max(caseData.casesCumulated.max, caseData.deathsCumulated.max));
    QtCharts::QChart *newChart = createChart(tr("Fälle kumuliert"), axisX, axisY);

    // add series and attach axes
    newChart->addSeries(sCases);
    sCases->attachAxis(axisX);
    sCases->attachAxis(axisY);

    newChart->addSeries(sDeaths);
    sDeaths->attachAxis(axisX);
    sDeaths->attachAxis(axisY);

    // add chart to its own view
    connectMarkers(*newChart);
    addNewChartView(newChart, ChartType::Cumulated);
}

void ChartWidget::initDailyChart()
{
    // init series
    QtCharts::QLineSeries *sCases = new QtCharts::QLineSeries();
    sCases->setPen(QPen(colors::ChartColors[2], 2));
    sCases->setName(tr("Erkrankungen"));
    QtCharts::QLineSeries *sDeaths = new QtCharts::QLineSeries();
    sDeaths->setPen(QPen(colors::ChartColors[0], 2));
    sDeaths->setName(tr("Todesfälle"));
    QtCharts::QLineSeries *sAverage = new QtCharts::QLineSeries();
    sAverage->setPen(QPen(colors::ChartColors[1], 2));
    sAverage->setName(tr("7-Tage-Inzidenz"));

    // init series data
    for(int i = 0; i < timestamps.size(); ++i) {
        sCases->append(timestamps[i].toMSecsSinceEpoch(), caseData.cases.series[i]);
        sDeaths->append(timestamps[i].toMSecsSinceEpoch(), caseData.deaths.series[i]);
        sAverage->append(timestamps[i].toMSecsSinceEpoch(), caseData.casesSevenDayAverage.series[i]);
    }

    // get maximum values from series
    sCases->setProperty("min", caseData.cases.min);
    sCases->setProperty("max", caseData.cases.max);
    sDeaths->setProperty("min", caseData.deaths.min);
    sDeaths->setProperty("max", caseData.deaths.max);
    sAverage->setProperty("min", caseData.casesSevenDayAverage.min);
    sAverage->setProperty("max", caseData.casesSevenDayAverage.max);

    // create chart elements
    QtCharts::QDateTimeAxis *axisX = createDateTimeAxis();
    QtCharts::QValueAxis *axisY = createValueAxis(1.1f * std::min(std::min(caseData.cases.min, caseData.deaths.min),
                                                                  static_cast<int>(caseData.casesSevenDayAverage.min)),
                                                  1.1f * std::max(std::max(caseData.cases.max, caseData.deaths.max),
                                                                  static_cast<int>(caseData.casesSevenDayAverage.max)));
    QtCharts::QChart *newChart = createChart(tr("Fälle / Tag"), axisX, axisY);

    // attach axes
    newChart->addSeries(sCases);
    sCases->attachAxis(axisX);
    sCases->attachAxis(axisY);

    newChart->addSeries(sDeaths);
    sDeaths->attachAxis(axisX);
    sDeaths->attachAxis(axisY);

    newChart->addSeries(sAverage);
    sAverage->attachAxis(axisX);
    sAverage->attachAxis(axisY);

    // add chart to its own view
    connectMarkers(*newChart);
    addNewChartView(newChart, ChartType::Daily);
}

void ChartWidget::initAccelerationChart()
{
    // init series
    QtCharts::QLineSeries *sAccCases = new QtCharts::QLineSeries();
    sAccCases->setPen(QPen(colors::ChartColors[2], 2));
    sAccCases->setName(tr("Fälle"));
    QtCharts::QLineSeries *sAccDeaths = new QtCharts::QLineSeries();
    sAccDeaths->setPen(QPen(colors::ChartColors[0], 2));
    sAccDeaths->setName(tr("Todesfälle"));
    QtCharts::QLineSeries *sAccCases7= new QtCharts::QLineSeries();
    sAccCases7->setPen(QPen(colors::ChartColors[1], 2));
    sAccCases7->setName(tr("7-Tage-Inzidenz"));

    // calculate rate
    int maxValueC = 0;
    int minValueC = 0;
    int minValueD = 0;
    int maxValueD = 0;
    float minValueA = 0;
    float maxValueA = 0;
    sAccCases->append(timestamps[0].toMSecsSinceEpoch(), 0);
    sAccDeaths->append(timestamps[0].toMSecsSinceEpoch(), 0);
    sAccCases7->append(timestamps[0].toMSecsSinceEpoch(), 0);
    for(int i = 1; i < timestamps.size(); ++i) {
        int valueC = caseData.cases.series[i] - caseData.cases.series[i - 1];
        maxValueC = std::max(valueC, maxValueC);
        minValueC = std::min(valueC, minValueC);
        sAccCases->append(timestamps[i].toMSecsSinceEpoch(),
                          valueC);

        int valueD = caseData.deaths.series[i] - caseData.deaths.series[i - 1];
        maxValueD = std::max(valueD, maxValueD);
        minValueD = std::min(valueD, minValueD);
        sAccDeaths->append(timestamps[i].toMSecsSinceEpoch(),
                           valueD);

        float valueF = caseData.casesSevenDayAverage.series[i] - caseData.casesSevenDayAverage.series[i - 1];
        maxValueA = std::max(valueF, maxValueA);
        minValueA = std::min(valueF, minValueA);
        sAccCases7->append(timestamps[i].toMSecsSinceEpoch(),
                           valueF);
    }

    sAccCases->setProperty("min", minValueC);
    sAccCases->setProperty("max", maxValueC);
    sAccDeaths->setProperty("min", minValueD);
    sAccDeaths->setProperty("max", maxValueD);
    sAccCases7->setProperty("min", minValueA);
    sAccCases7->setProperty("max", maxValueA);

    // create chart elements
    QtCharts::QDateTimeAxis *axisX = createDateTimeAxis();
    QtCharts::QValueAxis *axisY = createValueAxis(1.1f * std::min(std::min(minValueC, minValueD),
                                                                  static_cast<int>(minValueA)),
                                                  1.1f * std::max(std::max(maxValueC, maxValueD),
                                                                  static_cast<int>(maxValueA)));
    QtCharts::QChart *newChart = createChart(tr("Beschleunigung / Tag"), axisX, axisY);

    // add series and attach axes
    newChart->addSeries(sAccCases);
    sAccCases->attachAxis(axisX);
    sAccCases->attachAxis(axisY);

    newChart->addSeries(sAccDeaths);
    sAccDeaths->attachAxis(axisX);
    sAccDeaths->attachAxis(axisY);

    newChart->addSeries(sAccCases7);
    sAccCases7->attachAxis(axisX);
    sAccCases7->attachAxis(axisY);

    // add chart to its own view
    connectMarkers(*newChart);
    addNewChartView(newChart, ChartType::Acceleration);
}

void ChartWidget::addNewChartView(QtCharts::QChart *chart, ChartType type)
{
    ChartView *view = new ChartView(chart);
    view->setRubberBand(QtCharts::QChartView::RectangleRubberBand);
    view->setRenderHint(QPainter::Antialiasing);
    chartContainer->insertWidget(type, view);
    chartSwitchCombo->addItem(chart->title(), type);

    // make callout connections
    auto seriesAll = chart->series();
    for(auto series : seriesAll) {
        if(series->type() == QtCharts::QAbstractSeries::SeriesTypeLine) {
            auto lineSeries = qobject_cast<QtCharts::QLineSeries*>(series);
            connect(lineSeries, &QtCharts::QLineSeries::hovered,
                    view, &ChartView::setTooltip);
        }
    }
}

void ChartWidget::connectMarkers(const QtCharts::QChart &chart)
{
    // connect handle function for clicked markers
    const auto markers = chart.legend()->markers();
    for(QtCharts::QLegendMarker *marker : markers) {
        // disconnect possible existing cinnection to avoid multiple connections
        disconnect(marker, &QtCharts::QLegendMarker::clicked,
                   this, &ChartWidget::handleMarkerClicked);
        connect(marker, &QtCharts::QLegendMarker::clicked,
                this, &ChartWidget::handleMarkerClicked);

        disconnect(marker, &QtCharts::QLegendMarker::clicked,
                   this, &ChartWidget::rescale);
        connect(marker, &QtCharts::QLegendMarker::clicked,
                this, &ChartWidget::rescale);
    }
}

int ChartWidget::getOptimalTickinterval(int maxValue)
{
    int exp = static_cast<int>(std::log10(maxValue / 2));
    return static_cast<int>(std::pow(10, exp));
}

int ChartWidget::getOptimalTickinterval(int minValue, int maxValue)
{
    int exp = static_cast<int>(std::log10((maxValue - minValue) / 2));
    return static_cast<int>(std::pow(10, exp));
}

void ChartWidget::initDateTimeVector(const QDate &startDate, const int values)
{
    timestamps = QVector<QDateTime>(values);
    for(int i = 0; i < values; ++i) {
        timestamps[i] = QDateTime(startDate.addDays(i), QTime(0, 0));
    }
}

void ChartWidget::calculateTimestamps(const QVector<const CovidDataTreeItem *> caseDataItems)
{
    QDate startDate = caseDataItems.first()->getCaseData().startDate;
    QDate endDate = startDate.addDays(caseDataItems.first()->getCaseData().cases.series.size() - 1);
    for(int i = 1; i < caseDataItems.size(); ++i) {
        // save the ealiest QDate in the series
        startDate = (startDate < caseDataItems[i]->getCaseData().startDate)
                        ? startDate
                        : caseDataItems[i]->getCaseData().startDate;

        // calculate end date
        // this is necessary, because not all data series might be updated at the same time
        // with this calculation, we ensure that da latest date will be saved
        QDate end = startDate.addDays(caseDataItems[i]->getCaseData().cases.series.size() - 1);

        endDate = (endDate > end) ? endDate : end;
    }

    // now calculate the timestamps
    initDateTimeVector(startDate, (startDate.daysTo(endDate) + 1)); // add 1 since the end day has data itself
}

void ChartWidget::initSimpleSeries(const QVector<const CovidDataTreeItem *> caseDataItems,
                                   const QString &title,
                                   ChartType chartType)
{
    const int seriesCount = caseDataItems.size();
    QVector<QtCharts::QLineSeries*> series(seriesCount, nullptr);

    // init series
    for(int s = 0; s < seriesCount; ++s) {
        series[s] = new QtCharts::QLineSeries();
        series[s]->setPen(QPen(colors::ChartColors[s], 2));
        series[s]->setName(caseDataItems[s]->getItemNameAlt());
    }

    // init data
    qreal minOfAll = 0;
    qreal maxOfAll = 0;
    QDate startOfTimeSeries = timestamps.first().date();
    for(int s = 0; s < seriesCount; ++s) {
        // init values
        qreal min = 0;
        qreal max = 0;

        // calculate timeoffet to begin data
        int t = startOfTimeSeries.daysTo(caseDataItems[s]->getCaseData().startDate);
        int v = 0;

        if(chartType == SevenDayCasesCompare) {
            const CaseSeries<float>& data = getSeriesReferenceFloat(caseDataItems[s]->getCaseData(), chartType);
            for(; t < timestamps.size() && v < data.series.size(); ++t) {
                qint64 timePoint = timestamps[t].toMSecsSinceEpoch();
                qreal value = data.series[v++];
                series[s]->append(timePoint, value);
            }
            min = data.min;
            max = data.max;
        } else if(chartType == CumulatedCasesCompare
                  || chartType == CumulatedDeathsCompare
                  || chartType == DailyCasesCompare
                  || chartType == DailyDeathsCompare) {
            const CaseSeries<int>& data = getSeriesReferenceInt(caseDataItems[s]->getCaseData(), chartType);
            for(; t < timestamps.size() && v < data.series.size(); ++t) {
                qint64 timePoint = timestamps[t].toMSecsSinceEpoch();
                qreal value = data.series[v++];
                series[s]->append(timePoint, value);
            }
            min = data.min;
            max = data.max;
        }

        series[s]->setProperty("min", min);
        series[s]->setProperty("max", max);

        minOfAll = qMin(min, minOfAll);
        maxOfAll = qMax(max, maxOfAll);
    }

    // create chart elements
    QtCharts::QDateTimeAxis *axisX = createDateTimeAxis();
    QtCharts::QValueAxis *axisY = createValueAxis(1.1f * minOfAll,
                                                  1.1f * maxOfAll);
    QtCharts::QChart *newChart = createChart(title, axisX, axisY);

    // add series
    for(int s = 0; s < seriesCount; ++s) {
        newChart->addSeries(series[s]);
        series[s]->attachAxis(axisX);
        series[s]->attachAxis(axisY);
    }

    connectMarkers(*newChart);
    addNewChartView(newChart, chartType);
}

QtCharts::QDateTimeAxis* ChartWidget::createDateTimeAxis()
{
    QtCharts::QDateTimeAxis *axis = new QtCharts::QDateTimeAxis;

    int tickCount = timestamps.size() / 14 + 1;
    axis->setFormat("dd. MM");
    axis->setLabelsAngle(-60);
    axis->setTickCount(tickCount);

    return axis;
}

QtCharts::QValueAxis *ChartWidget::createValueAxis(qreal min, qreal max)
{
    QtCharts::QValueAxis *axis = new QtCharts::QValueAxis;

    axis->setLabelFormat("%i");
    axis->setTickType(QtCharts::QValueAxis::TickType::TicksDynamic);
    axis->setRange(1.1f * min, 1.1f * max);
    axis->setTickAnchor(0);
    axis->setTickInterval(getOptimalTickinterval(axis->max()));

    return axis;
}

QtCharts::QChart *ChartWidget::createChart(const QString &title,
                                           QtCharts::QAbstractAxis *xAxis,
                                           QtCharts::QAbstractAxis *yAxis)
{
    QtCharts::QChart *chart = new QtCharts::QChart();

    QFont font = chart->titleFont();
    font.setBold(true);
    font.setPointSizeF(font.pointSizeF() * 1.25f);
    chart->setTitleFont(font);
    chart->setTitle(title);
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);

    return chart;
}

const CaseSeries<int> &ChartWidget::getSeriesReferenceInt(const CaseData &data,
                                                          ChartWidget::ChartType chartType)
{
    switch(chartType) {
        case CumulatedCasesCompare:
            return data.casesCumulated;
            break;
        case CumulatedDeathsCompare:
            return data.deathsCumulated;
            break;
        case DailyCasesCompare:
            return data.cases;
            break;
        case DailyDeathsCompare:
            return data.deaths;
            break;
        default:
            return data.cases;
            break;
    }
}

const CaseSeries<float> &ChartWidget::getSeriesReferenceFloat(const CaseData &data,
                                                              ChartWidget::ChartType chartType)
{
    switch(chartType) {
        case SevenDayCasesCompare:
            return data.casesSevenDayAverage;
            break;
        default:
            return data.casesSevenDayAverage;
            break;
    }
}
