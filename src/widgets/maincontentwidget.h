#ifndef MAINCONTENTWIDGET_H
#define MAINCONTENTWIDGET_H

// Qt
#include <QWidget>

// Custom classes
#include "../data/coviddata.h"

// Qt Widgets
class QSplitter;
class QTabWidget;
class QStackedWidget;
class QPushButton;
class QToolBar;
class QButtonGroup;
class QProgressDialog;

// Custom Widgets
class TreeView;
class MapWidget;
class DownloadManager;

// Custom classes
class AppSettings;
class CovidDataTreeModel;

class MainContentWidget : public QWidget
{
    Q_OBJECT

    enum StackedIndex {
        Chart   = 0,
        Map     = 1
    };

public:
    explicit MainContentWidget(AppSettings &settings, QWidget *parent = nullptr);

    void initUi();

signals:

public slots:
    void downloadAndUpdate();
    void increaseProgressValue();
    void downloadsFinished();
    void addNewChart(const QModelIndex &index);
    void addNewChart(const CovidDataTreeItem *dataItem);
    void removeTab(int index);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    void loadData();

private:
    // Widgets
    // Qt
    QSplitter       *horizontalSplitter;
    QTabWidget      *chartTabWidget;
    QStackedWidget  *contentWidget;
    QPushButton     *mapButton;
    QPushButton     *chartButton;
    QPushButton     *updateButton;
    QToolBar        *contentToolBar;
    QButtonGroup    *contentButtonGroup;

    // Custom
    TreeView        *dataTreeView;
    MapWidget       *mapWidget;
    DownloadManager *downloadManager;

    AppSettings     &appSettings;
    QStringList     downloadFiles;

    CovidData           covidData;
    CovidDataTreeModel  *treeModel;

    // Download and load
    QProgressDialog *progress;
    int filesCompleted;


    QWidget *loadingWidget;
};

#endif // MAINCONTENTWIDGET_H
