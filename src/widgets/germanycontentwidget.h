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

class GermanyContentWidget : public QWidget
{
public:
    GermanyContentWidget(QWidget *parent = nullptr);

    bool loadGermanData(const QString &folder);

private:
    QWidget *leftTopLevelWidget;
    QTabWidget *tabWidget;
    QSplitter *splitter;
    QStackedWidget *contentStackedWidget;
    QTreeView *treeView;
    QComboBox *contentSelectCombo;
    QWidget *mapWidget;

    GermanyDataTreeModel *model;

    germany::Germany germany;
};

#endif // GERMANYCONTENTWIDGET_H
