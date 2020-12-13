#include "mainwindow.h"
#include "maincontentwidget.h"

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
#include <QStandardPaths>
#include <QCoreApplication>
#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    appSettings = new AppSettings(this);

    // get path to Application data
    appSavePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    appSettings->downloadFolder = QString("%1/downloads/").arg(appSavePath);

    readSettings();

    // set language and install translator, before initializing the widgets
    translator = new QTranslator(this);
    installTranslation();

    // init ui
    initWidgets();

    // connect the language chaged slot, after the ui initialization
    connect(appSettings, &AppSettings::languageChanged, this, &MainWindow::translateApp);
}

MainWindow::~MainWindow()
{

}

AppSettings& MainWindow::getAppSettings()
{
    return *appSettings;
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

void MainWindow::translateApp()
{
    installTranslation();

}

void MainWindow::readSettings()
{
    // use path in %APPDATA%/APPNAME/ (for Windows)
    QSettings settings(QString("%1/%2.ini").arg(appSavePath, QCoreApplication::applicationName()),
                       QSettings::IniFormat);

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    const QSize initialWindowSize = QSize(800, 600);

    // load settings
    settings.beginGroup("app");
    appSettings->setLanguageCode(settings.value("lang", QLocale::system().name()).toString());
    settings.endGroup();
    settings.beginGroup("mainwindow");
    resize(settings.value("size", initialWindowSize).toSize());
    move(settings.value("pos", QPoint((desktopRect.width() - initialWindowSize.width()) / 2,
                                      (desktopRect.height() - initialWindowSize.height()) / 2)).toPoint());
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    // use path in %APPDATA%/APPNAME/ (for Windows)
    QSettings settings(QString("%1/%2.ini").arg(appSavePath, QCoreApplication::applicationName()),
                       QSettings::IniFormat);

    // write settings
    settings.beginGroup("app");
    settings.setValue("lang", appSettings->getLanguageCode());
    settings.endGroup();
    settings.beginGroup("mainwindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::initWidgets()
{
    mainContentWidget = new MainContentWidget(*appSettings);
    mainContentWidget->show();
    this->setCentralWidget(mainContentWidget);
}

bool MainWindow::userReallyWantsToQuit()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  tr("Anwendung schließen"),
                                  tr("Möchten Sie die Anwendung wirklich schließen?"),
                                  QMessageBox::Yes | QMessageBox::No);
    return (reply == QMessageBox::Yes);
}

void MainWindow::installTranslation()
{
    if(translator->load("CovidDataVisualization_" + appSettings->getLanguageCode(), ":/languages/languages/")) {
        QCoreApplication::installTranslator(translator);
    }
}
