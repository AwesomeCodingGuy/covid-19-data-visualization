#include "chartwidget.h"

#include <algorithm>
#include <cmath>

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

    // attach axes
    sCases->attachAxis(axisX);
    sCases->attachAxis(axisY);
    sDeaths->attachAxis(axisX);
    sDeaths->attachAxis(axisY);
    sAverage->attachAxis(axisY);
    sAverage->attachAxis(axisX);

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
    sAccCases->append(timestamps[0].toMSecsSinceEpoch(), 0);
    sAccDeaths->append(timestamps[0].toMSecsSinceEpoch(), 0);
    sAccCases7->append(timestamps[0].toMSecsSinceEpoch(), 0);
    for(int i = 1; i < timestamps.size(); ++i) {
        sAccCases->append(timestamps[i].toMSecsSinceEpoch(),
                          caseData.cases[i] - caseData.cases[i - 1]);
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

int ChartWidget::getOptimalTickinterval(int maxValue)
{
    int exp = static_cast<int>(std::log10(maxValue / 2));
    return static_cast<int>(std::pow(10, exp));
}
