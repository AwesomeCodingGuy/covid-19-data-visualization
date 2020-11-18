#include "worldcontentwidget.h"

#include "../models/worlddatatreemodel.h"
#include "chartwidget.h"

#include <QDebug>
#include <QComboBox>
#include <QTreeView>
#include <QSplitter>
#include <QTabWidget>
#include <QStackedWidget>
#include <QBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

WorldContentWidget::WorldContentWidget(QWidget *parent)
    : QWidget(parent)
{
    // build left splitter part
    contentSelectCombo = new QComboBox();
    contentSelectCombo->addItem(tr("Diagramme"), 0);
//    contentSelectCombo->addItem(tr("Karte"), 1);

    treeView = new QTreeView();
    treeView->setIndentation(10);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(contentSelectCombo);
    hLayout->addStretch(1);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addWidget(treeView);
    vLayout->setContentsMargins(0, 0, 0 ,0);

    leftTopLevelWidget = new QWidget();
    leftTopLevelWidget->setLayout(vLayout);

    // build right splitter part
    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);
    mapWidget = new QWidget;
    contentStackedWidget = new QStackedWidget();
    contentStackedWidget->insertWidget(0, tabWidget);
//    contentStackedWidget->insertWidget(1, mapWidget);

    // build splitter and add to widgets layout
    splitter = new QSplitter;
    splitter->addWidget(leftTopLevelWidget);
    splitter->addWidget(contentStackedWidget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(splitter);

    setLayout(layout);

    // make connections
    connect(contentSelectCombo, SIGNAL(currentIndexChanged(int)),
            contentStackedWidget, SLOT(setCurrentIndex(int)));
    connect(treeView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(addNewChart(const QModelIndex &)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)),
            this, SLOT(removeTab(int)));
}

bool WorldContentWidget::loadWorldData(const QString &folder)
{
    bool ret = world.loadData(folder);

    // if data load was successfully load the tree model into the view
    if(ret) {
        model = new WorldDataTreeModel(world);
        treeView->setModel(model);
        treeView->header()->resizeSection(0, 240);
    }

    return ret;
}

void WorldContentWidget::removeTab(int index)
{
    tabWidget->removeTab(index);
}

void WorldContentWidget::addNewChart(const QModelIndex &index)
{
    if(contentSelectCombo->currentIndex() == 0) {
        QPair<QString, QString> keyData = model->key(index, Qt::DisplayRole);

        QVector<QDateTime> timestamps;
        CaseData data;

        if(!world.getCaseDataByName(keyData.first, keyData.second, data, timestamps)) {
            QMessageBox::information(this, tr("Fehler"),
                                     tr("Der Datensatz ist fehlerhaft. Das Diagramm kann nicht geladen werden."));
            return;
        }

        if(data.cases.isEmpty()) {
            QMessageBox::information(this, tr("Information"),
                                     tr("Der Datensatz ist leer. Das Diagramm kann nicht geladen werden."));
            return;
        }

        // Add Chart to QTabWidget
        ChartWidget *chartWidget = new ChartWidget(timestamps, data);
        int newIndex = tabWidget->addTab(chartWidget, (keyData.second.isEmpty()
                                                           ? keyData.first
                                                           : keyData.second));
        tabWidget->setCurrentIndex(newIndex);
    }
}
