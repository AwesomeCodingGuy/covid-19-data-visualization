#include "mainwindow.h"

#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QWidget>
#include <QActionGroup>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initWidgets();
    createActions();
    createMenus();

    resize(800, 600);
}

MainWindow::~MainWindow()
{

}

void MainWindow::switchView(QAction *action)
{
    ViewIndex viewIdx = actionMap.value(action, ViewIndex::Error);

    if(viewIdx != ViewIndex::Error) {
        centralStackedWidget->setCurrentIndex(static_cast<int>(viewIdx));
    }
}

void MainWindow::about()
{
    qDebug() << "Not yet implemented";
}

void MainWindow::initWidgets()
{
    centralStackedWidget = new QStackedWidget();
    this->setCentralWidget(centralStackedWidget);

    germanyContentWidget = new QWidget();
    germanyContentWidget->setStyleSheet("Background-color: #000000");
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

    centralStackedWidget->setCurrentIndex(static_cast<int>(ViewIndex::Germany));
}

void MainWindow::createActions()
{
    // File menu
    quitAction = new QAction(tr("&Beenden"));
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);

    // View menu
    germanyViewAction = new QAction(tr("&Deutschland"));
    worldViewAction = new QAction(tr("&Welt"));
    americaViewAction = new QAction(tr("&Amerika"));

    // Help menu
    aboutAction = new QAction(tr("&Über"));

    // ActionGroup for views
    viewGroup = new QActionGroup(this);
    viewGroup->addAction(germanyViewAction);
    viewGroup->addAction(worldViewAction);
    viewGroup->addAction(americaViewAction);
    germanyViewAction->setChecked(true);
    connect(viewGroup, &QActionGroup::triggered,
            this, &MainWindow::switchView);

    // ActionMap
    actionMap.insert(germanyViewAction, ViewIndex::Germany);
    actionMap.insert(worldViewAction, ViewIndex::World);
    actionMap.insert(americaViewAction, ViewIndex::America);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Datei"));
    fileMenu->addAction(quitAction);

    viewMenu = menuBar()->addMenu(tr("&Ansicht"));
    viewMenu->addAction(germanyViewAction);
    viewMenu->addAction(worldViewAction);
    viewMenu->addAction(americaViewAction);

    helpMenu = menuBar()->addMenu(tr("&Hilfe"));
    helpMenu->addAction(aboutAction);
}
