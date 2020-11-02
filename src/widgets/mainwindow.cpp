#include "mainwindow.h"
#include "dashboard.h"
#include "germanycontentwidget.h"

#include <QSettings>
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QWidget>
#include <QActionGroup>
#include <QStackedWidget>
#include <QCloseEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    readSettings();

    initWidgets();
    createActions();
    createMenus();

    resize(800, 600);
}

MainWindow::~MainWindow()
{

}

AppSettings& MainWindow::getAppSettings()
{
    return appSettings;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(userReallyWantsToQuit()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::switchView(QAction *action)
{
    ViewIndex viewIdx = actionMap.value(action, ViewIndex::Dashboard);
    centralStackedWidget->setCurrentIndex(static_cast<int>(viewIdx));
}

void MainWindow::about()
{
    qDebug() << "Not yet implemented";
}

void MainWindow::loadData()
{
    bool ret = germanyContentWidget->loadGermanData(appSettings.germanyDataPath);
    germanyViewAction->setEnabled(ret);

    qDebug() << "Loaded: " << ret;
}

void MainWindow::readSettings()
{
    // load settings
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "AwesomeCodingGuy", "covid-19-data-visualization");

    settings.beginGroup("dashboard");
    appSettings.germanyDataPath = settings.value("germanyDataPath", QString()).toString();
    appSettings.jhuDataPath = settings.value("jhuDataPath", QString()).toString();
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    // write settings
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "AwesomeCodingGuy", "covid-19-data-visualization");

    settings.beginGroup("dashboard");
    settings.setValue("germanyDataPath", appSettings.germanyDataPath);
    settings.setValue("jhuDataPath", appSettings.jhuDataPath);
    settings.endGroup();
}

void MainWindow::initWidgets()
{
    centralStackedWidget = new QStackedWidget();
    this->setCentralWidget(centralStackedWidget);

    dashboardWidget = new Dashboard(appSettings);
    centralStackedWidget->insertWidget(static_cast<int>(ViewIndex::Dashboard),
                                       dashboardWidget);
    connect(dashboardWidget, &Dashboard::sigLoadData,
            this, &MainWindow::loadData);

    germanyContentWidget = new GermanyContentWidget();
    centralStackedWidget->insertWidget(static_cast<int>(ViewIndex::Germany),
                                       germanyContentWidget);
    worldContentWidget = new QWidget();
    worldContentWidget->setStyleSheet("Background-color: #00ff00");
    centralStackedWidget->insertWidget(static_cast<int>(ViewIndex::World),
                                       worldContentWidget);
    americaContentWidget = new QWidget();
    americaContentWidget->setStyleSheet("Background-color: #0000ff");
    centralStackedWidget->insertWidget(static_cast<int>(ViewIndex::America),
                                       americaContentWidget);

    centralStackedWidget->setCurrentIndex(static_cast<int>(ViewIndex::Dashboard));
}

void MainWindow::createActions()
{
    // File menu
    quitAction = new QAction(tr("&Beenden"));
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);

    // View menu
    dashboardViewAction = new QAction(tr("Dash&board"));
    dashboardViewAction->setCheckable(true);
    germanyViewAction = new QAction(tr("&Deutschland"));
    germanyViewAction->setCheckable(true);
    germanyViewAction->setEnabled(false);
    worldViewAction = new QAction(tr("&Welt"));
    worldViewAction->setCheckable(true);
    worldViewAction->setEnabled(false);
    americaViewAction = new QAction(tr("&Amerika"));
    americaViewAction->setCheckable(true);
    americaViewAction->setEnabled(false);

    // Help menu
    aboutAction = new QAction(tr("&Über"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

    // ActionGroup for views
    viewGroup = new QActionGroup(this);
    viewGroup->addAction(dashboardViewAction);
    viewGroup->addAction(germanyViewAction);
    viewGroup->addAction(worldViewAction);
    viewGroup->addAction(americaViewAction);
    dashboardViewAction->setChecked(true);
    connect(viewGroup, &QActionGroup::triggered,
            this, &MainWindow::switchView);

    // ActionMap
    actionMap.insert(dashboardViewAction, ViewIndex::Dashboard);
    actionMap.insert(germanyViewAction, ViewIndex::Germany);
    actionMap.insert(worldViewAction, ViewIndex::World);
    actionMap.insert(americaViewAction, ViewIndex::America);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Datei"));
    fileMenu->addAction(quitAction);

    viewMenu = menuBar()->addMenu(tr("&Ansicht"));
    viewMenu->addAction(dashboardViewAction);
    viewMenu->addAction(germanyViewAction);
    viewMenu->addAction(worldViewAction);
    viewMenu->addAction(americaViewAction);

    helpMenu = menuBar()->addMenu(tr("&Hilfe"));
    helpMenu->addAction(aboutAction);
}

bool MainWindow::userReallyWantsToQuit()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Anwendung schließen",
                                  "Möchten Sie die Anwendung wirklich schließen?",
                                  QMessageBox::Yes | QMessageBox::No);
    return (reply == QMessageBox::Yes);
}
