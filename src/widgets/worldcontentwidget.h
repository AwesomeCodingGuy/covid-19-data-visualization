#ifndef WORLDCONTENTWIDGET_H
#define WORLDCONTENTWIDGET_H

#include <QWidget>

#include "../data/world.h"

class QComboBox;
class QTreeView;
class QSplitter;
class QStackedWidget;
class QTabWidget;
class QAbstractItemModel;

class WorldDataTreeModel;

class WorldContentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WorldContentWidget(QWidget *parent = nullptr);

    bool loadWorldData(const QString &folder);

public slots:
    void removeTab(int index);
    void addNewChart(const QModelIndex &index);

signals:

private:
    QWidget *leftTopLevelWidget;
    QTabWidget *tabWidget;
    QSplitter *splitter;
    QStackedWidget *contentStackedWidget;
    QTreeView *treeView;
    QComboBox *contentSelectCombo;
    QWidget *mapWidget;

    WorldDataTreeModel *model;

    world::World world;

};

#endif // WORLDCONTENTWIDGET_H
