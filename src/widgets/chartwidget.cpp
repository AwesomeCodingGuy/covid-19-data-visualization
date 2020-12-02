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
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegendMarker>

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
    timestamps = QVector<QDateTime>(caseData.cases.size());
    const QDate start(caseData.startDate);
    for(int i = 0; i < timestamps.size(); ++i) {
        timestamps[i] = QDateTime(start.addDays(i));
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

static bool yPointCompareMax(const QPointF &a, const QPointF &b)
{
    return (a.y() < b.y());
}

static bool yPointCompareMin(const QPointF &a, const QPointF &b)
{
    return (a.y() > b.y());
}

void ChartWidget::rescale()
{
    // TODO: somewhere is a bug that suddenly allocates much memory and the applications stays
    // unresponsible. The problem should be solved on anotehr way -> maybe save all min/max values when creating the window

    return; // added to skip the bug


    const QtCharts::QChartView * cView = qobject_cast<QtCharts::QChartView*>(chartContainer->widget(chartContainer->currentIndex()));

    if(!cView) {
        return;
    }

    const QtCharts::QChart *currentChart = cView->chart();

    qreal min = std::numeric_limits<qreal>::max();
    qreal max = std::numeric_limits<qreal>::min();
    for(QtCharts::QAbstractSeries *s : currentChart->series()) {
        QtCharts::QLineSeries *line = qobject_cast<QtCharts::QLineSeries*>(s);

        if(!line || !line->isVisible()) {
            continue;
        }

        auto vec = line->pointsVector();
        max = std::max(max, std::max_element(vec.begin(),
                                             vec.end(),
                                             yPointCompareMax)->y());

        min = std::min(min, std::max_element(vec.begin(),
                                             vec.end(),
                                             yPointCompareMin)->y());
    }

    for(auto *axis : currentChart->axes()) {
        if(axis && axis->type() == QtCharts::QAbstractAxis::AxisTypeValue) {
            auto a = qobject_cast<QtCharts::QValueAxis*>(axis);
            if(a) {
                a->setTickAnchor(0);
                a->setTickInterval(getOptimalTickinterval(max));
                a->setRange(min, max);
            }
        }
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
        sCases->append(timestamps[i].toMSecsSinceEpoch(), caseData.casesCumulated[i]);
        sDeaths->append(timestamps[i].toMSecsSinceEpoch(), caseData.deathsCumulated[i]);
    }
    int maxValue = caseData.casesCumulated.last();

    // init axes
    int tickCount = timestamps.size() / 14 + 1;
    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
    axisX->setFormat("dd. MM");
    axisX->setLabelsAngle(-60);
    axisX->setTickCount(tickCount);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTickType(QtCharts::QValueAxis::TickType::TicksDynamic);
    axisY->setTickAnchor(0);
    axisY->setTickInterval(getOptimalTickinterval(maxValue));
    axisY->setMin(0);

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
        sCases->append(timestamps[i].toMSecsSinceEpoch(), caseData.cases[i]);
        sDeaths->append(timestamps[i].toMSecsSinceEpoch(), caseData.deaths[i]);
        sAverage->append(timestamps[i].toMSecsSinceEpoch(), caseData.casesSevenDayAverage[i]);
    }

    // get maximum value from series
    int maxValue = std::max(*std::max_element(caseData.cases.begin(),
                                              caseData.cases.end()),
                            *std::max_element(caseData.deaths.begin(),
                                              caseData.deaths.end()));

    // init axis
    int tickCount = timestamps.size() / 14 + 1;
    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
    axisX->setFormat("dd. MM");
    axisX->setLabelsAngle(-60);
    axisX->setTickCount(tickCount);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTickType(QtCharts::QValueAxis::TickType::TicksDynamic);
    axisY->setTickAnchor(0);
    axisY->setTickInterval(getOptimalTickinterval(maxValue));
    axisY->setMin(0);
    axisY->setMax(maxValue);

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
    int maxValue = 0;
    int minValue = 0;
    sAccCases->append(timestamps[0].toMSecsSinceEpoch(), 0);
    sAccDeaths->append(timestamps[0].toMSecsSinceEpoch(), 0);
    sAccCases7->append(timestamps[0].toMSecsSinceEpoch(), 0);
    for(int i = 1; i < timestamps.size(); ++i) {
        int value = caseData.cases[i] - caseData.cases[i - 1];
        maxValue = std::max(value, maxValue);
        minValue = std::min(value, minValue);
        sAccCases->append(timestamps[i].toMSecsSinceEpoch(),
                          value);
        sAccDeaths->append(timestamps[i].toMSecsSinceEpoch(),
                           caseData.deaths[i] - caseData.deaths[i - 1]);
        sAccCases7->append(timestamps[i].toMSecsSinceEpoch(),
                           caseData.casesSevenDayAverage[i] - caseData.casesSevenDayAverage[i - 1]);
    }

    // init axis
    int tickCount = timestamps.size() / 14 + 1;
    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
    axisX->setFormat("dd. MM");
    axisX->setLabelsAngle(-60);
    axisX->setTickCount(tickCount);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTickType(QtCharts::QValueAxis::TickType::TicksDynamic);
    axisY->setTickAnchor(0);
    axisY->setTickInterval(getOptimalTickinterval(maxValue));
    axisY->setRange(minValue, maxValue);

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

    axisY->applyNiceNumbers();

    // add chart to its own view
    addNewChartView(accelerationChartView, accelerationChart, ChartType::Acceleration);
}

void ChartWidget::addNewChartView(QtCharts::QChartView *view, QtCharts::QChart *chart, ChartType type)
{
    view = new QtCharts::QChartView(chart);
    view->setRubberBand(QtCharts::QChartView::RectangleRubberBand);
    view->setRenderHint(QPainter::Antialiasing);
    chartContainer->insertWidget(type, view);
    chartSwitchCombo->addItem(chart->title(), type);
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
