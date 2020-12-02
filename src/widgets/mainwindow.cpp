#include "mainwindow.h"
#include "dashboard.h"
#include "germanycontentwidget.h"
#include "worldcontentwidget.h"
#include "usacontentwidget.h"
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
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // get path to Application data
    appSavePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    appSettings.downloadFolder = QString("%1/downloads/").arg(appSavePath);

    readSettings();

    initWidgets();
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

void MainWindow::readSettings()
{
    // use path in %APPDATA%/APPNAME/ (for Windows)
    QSettings settings(QString("%1/%2.ini").arg(appSavePath).arg(QCoreApplication::applicationName()),
                       QSettings::IniFormat);

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    const QSize initialWindowSize = QSize(800, 600);

    // load settings
    settings.beginGroup("mainwindow");
    resize(settings.value("size", initialWindowSize).toSize());
    move(settings.value("pos", QPoint((desktopRect.width() - initialWindowSize.width()) / 2,
                                      (desktopRect.height() - initialWindowSize.height()) / 2)).toPoint());
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    // use path in %APPDATA%/APPNAME/ (for Windows)
    QSettings settings(QString("%1/%2.ini").arg(appSavePath).arg(QCoreApplication::applicationName()),
                       QSettings::IniFormat);

    // write settings
    settings.beginGroup("mainwindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::initWidgets()
{
    mainContentWidget = new MainContentWidget(appSettings);
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
