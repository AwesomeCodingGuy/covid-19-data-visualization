#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class QMenu;
class QAction;
class QWidget;
class QActionGroup;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum class ViewIndex {
        Error   = -1,
        Germany = 0,
        World   = 1,
        America = 2
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchView(QAction *action);
    void about();

private:
    void initWidgets();
    void createActions();
    void createMenus();

    // Menus
    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    // ActionGroup
    QActionGroup *viewGroup;

    // Actions
    QAction *quitAction;

    QAction *germanyViewAction;
    QAction *worldViewAction;
    QAction *americaViewAction;

    QAction *aboutAction;

    // Action map
    QMap<QAction*, ViewIndex> actionMap;

    // Widgets
    QStackedWidget *centralStackedWidget;
    QWidget *germanyContentWidget;
    QWidget *worldContentWidget;
    QWidget *americaContentWidget;
};
#endif // MAINWINDOW_H
