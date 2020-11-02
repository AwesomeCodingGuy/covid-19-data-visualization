#include "chartwidget.h"

#include <algorithm>
#include <cmath>

#include <QDebug>
#include <QStackedWidget>
#include <QComboBox>
#include <QBoxLayout>

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
    addCumulatedChart();
    addDailyChart();
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
    pen_1 = QPen(Qt::blue);
    pen_2 = QPen(Qt::black);
    pen_3 = QPen(QColor(255, 127, 0));
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
    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
    axisX->setFormat("dd. MMM yyyy");
    axisX->setTitleText(tr("Datum"));

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
    sCases->setName(tr("Kumulierte Fälle"));
    QtCharts::QLineSeries *sDeaths = new QtCharts::QLineSeries();
    sDeaths->setPen(pen_2);
    sDeaths->setName(tr("Kumulierte Todesfälle"));
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
    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
    axisX->setFormat("dd. MMM yyyy");
    axisX->setTitleText(tr("Datum"));

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

void ChartWidget::addCumulatedChart()
{
    cumulatedChartView = new QtCharts::QChartView(cumulatedChart);
    cumulatedChartView->setRenderHint(QPainter::Antialiasing);
    int idx = chartContainer->addWidget(cumulatedChartView);
    chartSwitchCombo->addItem(tr("Kumulierte Fälle"), idx);
}

void ChartWidget::addDailyChart()
{
    dailyChartView = new QtCharts::QChartView(dailyChart);
    dailyChartView->setRenderHint(QPainter::Antialiasing);
    int idx = chartContainer->addWidget(dailyChartView);
    chartSwitchCombo->addItem(tr("Tägliche Fälle"), idx);
}

int ChartWidget::getOptimalTickinterval(int maxValue)
{
    int exp = static_cast<int>(std::log10(maxValue / 2));

    return static_cast<int>(std::pow(10, exp));
}
