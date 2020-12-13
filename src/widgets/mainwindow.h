#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "../data/appsettings.h"

class QSettings;
class QMenu;
class QAction;
class QWidget;
class QActionGroup;
class QStackedWidget;
class QTranslator;

class AppSettings;

class MainContentWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum class ViewIndex {
        Dashboard   = 0,
        Germany     = 1,
        World       = 2,
        America     = 3
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    AppSettings& getAppSettings();

public slots:
    void closeEvent(QCloseEvent *event);

    void translateApp();

private slots:

private:
    void readSettings();
    void writeSettings();
    void initWidgets();
    bool userReallyWantsToQuit();
    void installTranslation();

    AppSettings *appSettings;
    QString appSavePath;

    QTranslator *translator;

    MainContentWidget *mainContentWidget;

    // Action map
    QMap<QAction*, ViewIndex> actionMap;
};
#endif // MAINWINDOW_H
