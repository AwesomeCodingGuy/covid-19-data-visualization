#include "maincontentwidget.h"

// Qt header
#include <QSplitter>
#include <QTabWidget>
#include <QStackedWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QToolBar>
#include <QIcon>
#include <QDebug>
#include <QStylePainter>
#include <QPainter>
#include <QProgressDialog>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

// Custom header
#include "treeview.h"
#include "mapwidget.h"
#include "chartwidget.h"
#include "../utils/downloadmanager.h"
#include "../data/constants.h"
#include "../data/appsettings.h"
#include "../data/reader/germanydatareader.h"
#include "../data/reader/usadatareader.h"
#include "../data/reader/worlddatareader.h"
#include "../models/coviddatatreemodel.h"

MainContentWidget::MainContentWidget(AppSettings &settings, QWidget *parent)
    : QWidget(parent)
    , appSettings(settings)
    , treeModel(nullptr)
    , progress(nullptr)
{
    // Ui
    initUi();

    // intern
    downloadFiles = QStringList({
        constants::germanyRepoDataUrl + constants::casesByState,
        constants::germanyRepoDataUrl + constants::casesByAgs,
        constants::germanyRepoDataUrl + constants::deathsByState,
        constants::germanyRepoDataUrl + constants::deathsByAgs,
        constants::germanyRepoDataUrl + constants::ags,
        constants::jhuRepoDataUrl + constants::casesUS,
        constants::jhuRepoDataUrl + constants::casesGlobal,
        constants::jhuRepoDataUrl + constants::deathsUS,
        constants::jhuRepoDataUrl + constants::deathsGlobal,
    });

    // init DownloadManager
    downloadManager = new DownloadManager(appSettings.downloadFolder, this);
    connect(downloadManager, &DownloadManager::fileFinished,
            this, &MainContentWidget::increaseProgressValue);
    connect(downloadManager, &DownloadManager::downloadsFinished,
            this, &MainContentWidget::downloadsFinished);


    resize(800, 600);

    QTimer::singleShot(1000, this, &MainContentWidget::loadData);
}

void MainContentWidget::initUi()
{
    // Widget initialization
    // Qt
    horizontalSplitter  = new QSplitter();
    chartTabWidget      = new QTabWidget();
    chartTabWidget->setTabsClosable(true);
    connect(chartTabWidget, SIGNAL(tabCloseRequested(int)),
            this, SLOT(removeTab(int)));
    contentWidget       = new QStackedWidget();
    mapButton           = new QPushButton(QIcon(":/icons/images/icons/map_32x32.png"),
                                          tr("Karten"));
    chartButton         = new QPushButton(QIcon(":/icons/images/icons/chart_32x32.png"),
                                          tr("Diagramme"));
    updateButton        = new QPushButton(QIcon(":/icons/images/icons/update_32x32.png"),
                                          tr("Daten herunterladen / aktualisieren"));
    contentToolBar      = new QToolBar();
    contentButtonGroup  = new QButtonGroup(this);

    // Custom
    dataTreeView        = new TreeView();
    connect(dataTreeView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(addNewChart(const QModelIndex &)));
    mapWidget           = new MapWidget();
    connect(mapWidget, &MapWidget::pathItemDoubleClicked,
            this, static_cast<void (MainContentWidget::*)(const CovidDataTreeItem *)>(&MainContentWidget::addNewChart));

// left of splitter
    connect(updateButton, &QPushButton::clicked,
            this, &MainContentWidget::downloadAndUpdate);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(dataTreeView);
    leftLayout->addWidget(updateButton);

    // left container widgte for layout
    QWidget *leftWidget = new QWidget();
    leftWidget->setLayout(leftLayout);
    auto margins = leftLayout->contentsMargins();
    margins.setTop(0);
    leftLayout->setContentsMargins(margins);

// right of splitter
    // ToolBar
    mapButton->setCheckable(true);
    chartButton->setCheckable(true);
    chartButton->setChecked(true);

    contentButtonGroup->addButton(chartButton, StackedIndex::Chart);
    contentButtonGroup->addButton(mapButton, StackedIndex::Map);
    contentButtonGroup->setExclusive(true);

    contentToolBar->addWidget(chartButton);
    contentToolBar->addWidget(mapButton);

    contentWidget->insertWidget(StackedIndex::Chart, chartTabWidget);
    contentWidget->insertWidget(StackedIndex::Map, mapWidget);
    connect(contentButtonGroup, &QButtonGroup::idClicked,
            contentWidget, &QStackedWidget::setCurrentIndex);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setMenuBar(contentToolBar);
    rightLayout->addWidget(contentWidget);

    // right container widgte for layout
    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(rightLayout);
    margins = rightLayout->contentsMargins();
    margins.setLeft(0);
    rightLayout->setContentsMargins(margins);

    // splitter
    horizontalSplitter->addWidget(leftWidget);
    horizontalSplitter->addWidget(rightWidget);
    horizontalSplitter->setStretchFactor(0, 0);
    horizontalSplitter->setStretchFactor(1, 1);

    // add splitter to widget
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(horizontalSplitter);

    // set Layout
    setLayout(hLayout);
    hLayout->setContentsMargins(0, 11, 0, 0);
}

void MainContentWidget::downloadAndUpdate()
{
    qDebug() << "Starting download";

    // create new progress dialog
    progress = new QProgressDialog(this);
    progress->setLabelText(tr("Dateien werden heruntergeladen..."));
    progress->setCancelButton(nullptr);
    progress->setWindowModality(Qt::WindowModal);
    progress->setRange(0, downloadFiles.size());
    progress->show();

    // start DownloadManager
    filesCompleted = 0;
    downloadManager->appendFiles(downloadFiles);
    downloadManager->startDownload();
}

void MainContentWidget::increaseProgressValue()
{
    progress->setValue(++filesCompleted);
}

void MainContentWidget::downloadsFinished()
{
    qDebug() << "Downloads finished";

    // delete progress window since it cannot be properly hidden
    if(progress) {
        delete progress;
    }

    // reload data
    covidData = CovidData();
    loadData();
}

void MainContentWidget::addNewChart(const QModelIndex &index)
{
    const CovidDataTreeItem *chartDataItem = treeModel->getCovidDataItem(index);

    addNewChart(chartDataItem);
}

void MainContentWidget::addNewChart(const CovidDataTreeItem *chartDataItem)
{
    if(chartDataItem) {
        // TODO : debug parent pointer errors in level 2
        // qDebug() << getCompleteName(chartDataItem);

        // Add Chart
        if(chartDataItem->getCaseData().cases.series.size() > 0) {
            ChartWidget *chartWidget = new ChartWidget(chartDataItem->getCaseData());
            int newIndex = chartTabWidget->addTab(chartWidget, chartDataItem->getItemNameAlt());
            chartTabWidget->setCurrentIndex(newIndex);
        } else {
            QMessageBox::warning(this, tr("Fehler"), tr("Keine Daten vorhanden"));
        }

        // switch to charts if not visible
        if(contentWidget->currentIndex() != StackedIndex::Chart) {
            chartButton->animateClick();
        }
    }
}

void MainContentWidget::removeTab(int index)
{
    chartTabWidget->removeTab(index);
}

void MainContentWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainContentWidget::loadData()
{
    //init data readers
    CovidDataTreeItem datasetGermanyItem;
    GermanyDataReader gReader(appSettings.downloadFolder, datasetGermanyItem);

    CovidDataTreeItem datasetUsaItem;
    UsaDataReader usaReader(appSettings.downloadFolder, datasetUsaItem);

    CovidDataTreeItem datasetWorldItem;
    WorldDataReader worldReader(appSettings.downloadFolder, datasetWorldItem);

    // first check if all files are available and report missing files.
    // this will happen on first start or if the files have been deleted
    if(!gReader.validateFiles()
        && !usaReader.validateFiles()
        && !worldReader.validateFiles()) {
        QMessageBox::information(this, tr("Information"),
                                 tr("Es sind keine Daten vorhanden.\n\n"
                                    "Bitte drücken sie den Button zum Herunterladen der aktuellen Daten."));
        return;
    } else if(!gReader.validateFiles()
                || !usaReader.validateFiles()
                || !worldReader.validateFiles()) {
        auto ret = QMessageBox::question(this, tr("Information"),
                                         tr("Es fehlen Daten. Die Anwendung kann mit den vorhandenen Daten fortfahren.\n\n"
                                            "Möchten Sie den Ladevorgang fortsetzen?"),
                                         QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                                         QMessageBox::StandardButton::No);
        if(ret != QMessageBox::StandardButton::Yes) {
            return;
        }
    }

    // TODO: Rework for the loading needed.
    // Calling "processEvents" here to keep the GUI updating the progressbar is bad style,
    // however an easy solution at this palce.

    // init progressdialog
    progress = new QProgressDialog(this);
    progress->setCancelButton(nullptr);
    progress->setWindowModality(Qt::WindowModal);
    progress->setRange(0, 4);
    progress->show();

    progress->setLabelText(tr("Datensätze werden gelesen...\n") + tr("Deutschland"));
    QCoreApplication::processEvents();

    // get german data from files
    if(gReader.loadData()) {
        covidData.addDataSet(datasetGermanyItem);
    }
    progress->setValue(1);
    progress->setLabelText(tr("Datensätze werden gelesen...\n") + tr("USA"));
    QCoreApplication::processEvents();

    // get usa data from files
    if(usaReader.loadData()) {
        covidData.addDataSet(datasetUsaItem);
    }
    progress->setValue(2);
    progress->setLabelText(tr("Datensätze werden gelesen...\n") + tr("World"));
    QCoreApplication::processEvents();

    // get world data from files
    if(worldReader.loadData()) {
        covidData.addDataSet(datasetWorldItem);
    }
    progress->setValue(3);
    progress->setLabelText(tr("Karten werden gemalt..."));
    QCoreApplication::processEvents();

    // load Mapdata
    mapWidget->loadMapData(covidData.getDataSets());
    progress->setValue(4);
    QCoreApplication::processEvents();

    // set data into tree view
    treeModel = new CovidDataTreeModel(covidData.getDataSets());
    dataTreeView->setModel(treeModel);

    // loading finished
    qDebug() << "Loading finished";
    if(progress) {
        delete progress;
    }
}
