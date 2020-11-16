#ifndef GERMANYCONTENTWIDGET_H
#define GERMANYCONTENTWIDGET_H

#include <QWidget>

#include "../data/germany.h"

class QComboBox;
class QTreeView;
class QSplitter;
class QStackedWidget;
class QTabWidget;
class QAbstractItemModel;

class GermanyDataTreeModel;
class GermanyMapWidget;

class GermanyContentWidget : public QWidget
{
    Q_OBJECT

public:
    GermanyContentWidget(QWidget *parent = nullptr);

    bool loadGermanData(const QString &folder);

public slots:
    void removeTab(int index);
    void addNewChart(const QModelIndex &index);
    void addNewChart(int ags);

private:
    QWidget *leftTopLevelWidget;
    QTabWidget *tabWidget;
    QSplitter *splitter;
    QStackedWidget *contentStackedWidget;
    QTreeView *treeView;
    QComboBox *contentSelectCombo;

    GermanyMapWidget *mapWidget;

    GermanyDataTreeModel *model;

    germany::Germany germany;
};

#endif // GERMANYCONTENTWIDGET_H
