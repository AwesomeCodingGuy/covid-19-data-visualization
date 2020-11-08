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

class Dashboard;
class AppSettings;
class WorldContentWidget;
class GermanyContentWidget;
class UsaContentWidget;

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

private slots:
    void switchView(QAction *action);
    void about();
    void loadData();

private:
    void readSettings();
    void writeSettings();
    void initWidgets();
    void createActions();
    void createMenus();
    bool userReallyWantsToQuit();

    AppSettings appSettings;

    // Menus
    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    // ActionGroup
    QActionGroup *viewGroup;

    // Actions
    QAction *quitAction;

    QAction *dashboardViewAction;
    QAction *germanyViewAction;
    QAction *worldViewAction;
    QAction *usaViewAction;

    QAction *aboutAction;

    // Action map
    QMap<QAction*, ViewIndex> actionMap;

    // Widgets
    QStackedWidget *centralStackedWidget;
    Dashboard *dashboardWidget;
    GermanyContentWidget *germanyContentWidget;
    WorldContentWidget *worldContentWidget;
    UsaContentWidget *usaContentWidget;
};
#endif // MAINWINDOW_H
