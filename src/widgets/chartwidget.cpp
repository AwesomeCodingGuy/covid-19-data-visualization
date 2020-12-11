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

ChartWidget::ChartWidget(const QVector<QDateTime> &timestamps,
                         const CaseData &caseData,
                         QWidget *parent)
    : QWidget(parent)
    , timestamps(timestamps)
    , caseData(caseData)
{
    // init pens for charts
    pen_1 = QPen(Qt::blue, 2);
    pen_2 = QPen(Qt::black, 2);
    pen_3 = QPen(QColor(255, 127, 0), 2);

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
    // init pens for charts
    pen_1 = QPen(Qt::blue, 2);
    pen_2 = QPen(Qt::black, 2);
    pen_3 = QPen(QColor(255, 127, 0), 2);

    // calculate timestamps
    timestamps = QVector<QDateTime>(caseData.cases.series.size());
    const QDate start(caseData.startDate);
    for(int i = 0; i < timestamps.size(); ++i) {
        timestamps[i] = QDateTime(start.addDays(i), QTime(0, 0));
    }

    // init UI
    initUi();

    // init charts
    initCumulatedChart();
    initDailyChart();
    initAccelerationChart();
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
    sCases->setPen(pen_1);
    sCases->setName(tr("Erkrankungen"));
    QtCharts::QLineSeries *sDeaths = new QtCharts::QLineSeries();
    sDeaths->setPen(pen_2);
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

    // init axes
    int tickCount = timestamps.size() / 14 + 1;
    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
    axisX->setFormat("dd. MM");
    axisX->setLabelsAngle(-60);
    axisX->setTickCount(tickCount);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTickType(QtCharts::QValueAxis::TickType::TicksDynamic);
    axisY->setRange(1.1f * std::min(caseData.casesCumulated.min, caseData.deathsCumulated.min),
                    1.1f * std::max(caseData.casesCumulated.max, caseData.deathsCumulated.max));
    axisY->setTickAnchor(0);
    axisY->setTickInterval(getOptimalTickinterval(axisY->max()));

    // build chart
    cumulatedChart = new QtCharts::QChart();
    QFont font = cumulatedChart->titleFont();
    font.setBold(true);
    font.setPointSizeF(font.pointSizeF() * 1.25f);
    cumulatedChart->setTitleFont(font);
    cumulatedChart->setTitle(tr("Fälle kumuliert"));
    cumulatedChart->addSeries(sCases);
    cumulatedChart->addSeries(sDeaths);
    cumulatedChart->addAxis(axisX, Qt::AlignBottom);
    cumulatedChart->addAxis(axisY, Qt::AlignLeft);

    connectMarkers(*cumulatedChart);

    // attach axes
    sCases->attachAxis(axisX);
    sCases->attachAxis(axisY);
    sDeaths->attachAxis(axisX);
    sDeaths->attachAxis(axisY);

    // add chart to its own view
    addNewChartView(cumulatedChartView, cumulatedChart, ChartType::Cumulated);
}

void ChartWidget::initDailyChart()
{
    // init series
    QtCharts::QLineSeries *sCases = new QtCharts::QLineSeries();
    sCases->setPen(pen_1);
    sCases->setName(tr("Erkrankungen"));
    QtCharts::QLineSeries *sDeaths = new QtCharts::QLineSeries();
    sDeaths->setPen(pen_2);
    sDeaths->setName(tr("Todesfälle"));
    QtCharts::QLineSeries *sAverage = new QtCharts::QLineSeries();
    sAverage->setPen(pen_3);
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

    // init axis
    int tickCount = timestamps.size() / 14 + 1;
    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
    axisX->setFormat("dd. MM");
    axisX->setLabelsAngle(-60);
    axisX->setTickCount(tickCount);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTickType(QtCharts::QValueAxis::TickType::TicksDynamic);
    axisY->setRange(1.1f * std::min(std::min(caseData.cases.min, caseData.deaths.min),
                                    static_cast<int>(caseData.casesSevenDayAverage.min)),
                    1.1f * std::max(std::max(caseData.cases.max, caseData.deaths.max),
                                    static_cast<int>(caseData.casesSevenDayAverage.max)));
    axisY->setTickAnchor(0);
    axisY->setTickInterval(getOptimalTickinterval(axisY->max()));

    // build chart
    dailyChart = new QtCharts::QChart();
    QFont font = dailyChart->titleFont();
    font.setBold(true);
    font.setPointSizeF(font.pointSizeF() * 1.25f);
    dailyChart->setTitleFont(font);
    dailyChart->setTitle(tr("Fälle / Tag"));
    dailyChart->addSeries(sCases);
    dailyChart->addSeries(sDeaths);
    dailyChart->addSeries(sAverage);
    dailyChart->addAxis(axisX, Qt::AlignBottom);
    dailyChart->addAxis(axisY, Qt::AlignLeft);

    connectMarkers(*dailyChart);

    // attach axes
    sCases->attachAxis(axisX);
    sCases->attachAxis(axisY);
    sDeaths->attachAxis(axisX);
    sDeaths->attachAxis(axisY);
    sAverage->attachAxis(axisX);
    sAverage->attachAxis(axisY);

    // axisY->applyNiceNumbers();

    // add chart to its own view
    addNewChartView(dailyChartView, dailyChart, ChartType::Daily);
}

void ChartWidget::initAccelerationChart()
{
    // init series
    QtCharts::QLineSeries *sAccCases = new QtCharts::QLineSeries();
    sAccCases->setPen(pen_1);
    sAccCases->setName(tr("Fälle"));
    QtCharts::QLineSeries *sAccDeaths = new QtCharts::QLineSeries();
    sAccDeaths->setPen(pen_2);
    sAccDeaths->setName(tr("Todesfälle"));
    QtCharts::QLineSeries *sAccCases7= new QtCharts::QLineSeries();
    sAccCases7->setPen(pen_3);
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

    // init axis
    int tickCount = timestamps.size() / 14 + 1;
    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
    axisX->setFormat("dd. MM");
    axisX->setLabelsAngle(-60);
    axisX->setTickCount(tickCount);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTickType(QtCharts::QValueAxis::TickType::TicksDynamic);
    axisY->setRange(1.1f * std::min(std::min(minValueC, minValueD), static_cast<int>(minValueA)),
                    1.1f * std::max(std::max(maxValueC, maxValueD), static_cast<int>(maxValueA)));
    axisY->setTickAnchor(0);
    axisY->setTickInterval(getOptimalTickinterval(axisY->max()));

    // build chart
    accelerationChart = new QtCharts::QChart();
    QFont font = accelerationChart->titleFont();
    font.setBold(true);
    font.setPointSizeF(font.pointSizeF() * 1.25f);
    accelerationChart->setTitleFont(font);
    accelerationChart->setTitle(tr("Beschleunigung / Tag"));
    accelerationChart->addSeries(sAccCases);
    accelerationChart->addSeries(sAccDeaths);
    accelerationChart->addSeries(sAccCases7);
    accelerationChart->addAxis(axisX, Qt::AlignBottom);
    accelerationChart->addAxis(axisY, Qt::AlignLeft);

    connectMarkers(*accelerationChart);

    // attach axes
    sAccCases->attachAxis(axisX);
    sAccCases->attachAxis(axisY);
    sAccDeaths->attachAxis(axisX);
    sAccDeaths->attachAxis(axisY);
    sAccCases7->attachAxis(axisX);
    sAccCases7->attachAxis(axisY);

    // axisY->applyNiceNumbers();

    // add chart to its own view
    addNewChartView(accelerationChartView, accelerationChart, ChartType::Acceleration);
}

void ChartWidget::addNewChartView(ChartView *view, QtCharts::QChart *chart, ChartType type)
{
    view = new ChartView(chart);
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
