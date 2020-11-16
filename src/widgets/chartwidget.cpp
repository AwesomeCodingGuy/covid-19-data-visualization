#include "chartwidget.h"

#include <algorithm>
#include <cmath>

#include <QBrush>
#include <QPen>
#include <QDebug>
#include <QStackedWidget>
#include <QComboBox>
#include <QBoxLayout>

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
    initUi();
    initGraphicalStuff();
    initCumulatedChart();
    initDailyChart();
    initAccelerationChart();
    addCumulatedChart();
    addDailyChart();
    addAccelerationChart();
}

ChartWidget::~ChartWidget()
{

}

void ChartWidget::initUi()
{
    // set stacked widget as main content widget
    chartContainer = new QStackedWidget();

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(chartContainer);

    setLayout(hLayout);

    // add a combo box to the widget above the stacked widget.
    chartSwitchCombo = new QComboBox(this);
    chartSwitchCombo->move(0, 0);
    connect(chartSwitchCombo, SIGNAL(currentIndexChanged(int)),
            chartContainer, SLOT(setCurrentIndex(int)));
}

void ChartWidget::initGraphicalStuff()
{
    pen_1 = QPen(Qt::blue, 2);
    pen_2 = QPen(Qt::black, 2);
    pen_3 = QPen(QColor(255, 127, 0), 2);
}

void ChartWidget::initCumulatedChart()
{
    // init series
    QtCharts::QLineSeries *sCases = new QtCharts::QLineSeries();
    sCases->setPen(pen_1);
    sCases->setName(tr("Kumulierte Fälle"));
    QtCharts::QLineSeries *sDeaths = new QtCharts::QLineSeries();
    sDeaths->setPen(pen_2);
    sDeaths->setName(tr("Kumulierte Todesfälle"));

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
    axisY->setTitleText(tr("Fälle"));
    axisY->setTickType(QtCharts::QValueAxis::TickType::TicksDynamic);
    axisY->setTickAnchor(0);
    axisY->setTickInterval(getOptimalTickinterval(maxValue));
    axisY->setMin(0);

    // build chart
    cumulatedChart = new QtCharts::QChart();
    cumulatedChart->addSeries(sCases);
    cumulatedChart->addSeries(sDeaths);
    cumulatedChart->addAxis(axisX, Qt::AlignBottom);
    cumulatedChart->addAxis(axisY, Qt::AlignLeft);

    // attach axes
    sCases->attachAxis(axisX);
    sCases->attachAxis(axisY);
    sDeaths->attachAxis(axisX);
    sDeaths->attachAxis(axisY);
}

void ChartWidget::initDailyChart()
{
    // init series
    QtCharts::QLineSeries *sCases = new QtCharts::QLineSeries();
    sCases->setPen(pen_1);
    sCases->setName(tr("Fälle"));
    QtCharts::QLineSeries *sDeaths = new QtCharts::QLineSeries();
    sDeaths->setPen(pen_2);
    sDeaths->setName(tr("Todesfälle"));
    QtCharts::QLineSeries *sAverage = new QtCharts::QLineSeries();
    sAverage->setPen(pen_3);
    sAverage->setName(tr("Fälle 7-Tage Mittelwert"));

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
    axisY->setTitleText(tr("Fälle"));
    axisY->setTickType(QtCharts::QValueAxis::TickType::TicksDynamic);
    axisY->setTickAnchor(0);
    axisY->setTickInterval(getOptimalTickinterval(maxValue));
    axisY->setMin(0);
    axisY->setMax(maxValue);

    // build chart
    dailyChart = new QtCharts::QChart();
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
}

void ChartWidget::initAccelerationChart()
{
    // init series
    QtCharts::QLineSeries *sAccCases = new QtCharts::QLineSeries();
    sAccCases->setPen(pen_1);
    sAccCases->setName(tr("Beschleunigungsrate der Fälle"));
    QtCharts::QLineSeries *sAccDeaths = new QtCharts::QLineSeries();
    sAccDeaths->setPen(pen_2);
    sAccDeaths->setName(tr("Beschleunigungsrate der Todesfälle"));
    QtCharts::QLineSeries *sAccCases7= new QtCharts::QLineSeries();
    sAccCases7->setPen(pen_3);
    sAccCases7->setName(tr("Beschleunigungsrate der 7-Tage-Inzidenz"));

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
    axisY->setTitleText(tr("Änderung"));
    axisY->applyNiceNumbers();
    // axisY->setTickType(QtCharts::QValueAxis::TickType::TicksDynamic);

    // build chart
    accelerationChart = new QtCharts::QChart();
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

    //
    axisY->applyNiceNumbers();

    // legend
    // QtCharts::QLegend *legend = accelerationChart->legend();
    // legend->detachFromChart();
    // legend->setBackgroundVisible(true);
    // legend->setBrush(QBrush(QColor(128, 128, 128, 128)));
    // legend->setPen(QPen(QColor(192, 192, 192, 192)));
    // legend->setGeometry(QRectF(width() / 2 - 150, 20, 300, 80));
}

void ChartWidget::addCumulatedChart()
{
    cumulatedChartView = new QtCharts::QChartView(cumulatedChart);
    cumulatedChartView->setRenderHint(QPainter::Antialiasing);
    chartContainer->insertWidget(ChartType::Cumulated, cumulatedChartView);
    chartSwitchCombo->addItem(tr("Kumulierte Fälle"), ChartType::Cumulated);
}

void ChartWidget::addDailyChart()
{
    dailyChartView = new QtCharts::QChartView(dailyChart);
    dailyChartView->setRenderHint(QPainter::Antialiasing);
    chartContainer->insertWidget(ChartType::Daily, dailyChartView);
    chartSwitchCombo->addItem(tr("Tägliche Fälle"), ChartType::Daily);
}

void ChartWidget::addAccelerationChart()
{
    accelerationChartView = new QtCharts::QChartView(accelerationChart);
    accelerationChartView->setRenderHint(QPainter::Antialiasing);
    chartContainer->insertWidget(ChartType::Acceleration, accelerationChartView);
    chartSwitchCombo->addItem(tr("Beschleunigung"), ChartType::Acceleration);
}

int ChartWidget::getOptimalTickinterval(int maxValue)
{
    int exp = static_cast<int>(std::log10(maxValue / 2));

    return static_cast<int>(std::pow(10, exp));
}
