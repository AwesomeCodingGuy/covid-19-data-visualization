#ifndef USACONTENTWIDGET_H
#define USACONTENTWIDGET_H

#include <QWidget>

#include "../data/usa.h"

class QComboBox;
class QTreeView;
class QSplitter;
class QStackedWidget;
class QTabWidget;
class QAbstractItemModel;

class UsaDataTreeModel;

class UsaContentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UsaContentWidget(QWidget *parent = nullptr);
    ~UsaContentWidget();

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

    UsaDataTreeModel *model;

    usa::Usa usa;
};

#endif // USACONTENTWIDGET_H
