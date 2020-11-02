#include "germanycontentwidget.h"

#include "../models/germanydatatreemodel.h"
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

GermanyContentWidget::GermanyContentWidget(QWidget *parent)
    : QWidget(parent)
{
    // build left splitter part
    contentSelectCombo = new QComboBox();
    contentSelectCombo->addItem(tr("Diagramme"), 0);
    contentSelectCombo->addItem(tr("Karte"), 1);

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
    contentStackedWidget->insertWidget(1, mapWidget);

    // build splitter and add to widgets layout
    splitter = new QSplitter;
    splitter->addWidget(leftTopLevelWidget);
    splitter->addWidget(contentStackedWidget);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(splitter);

    setLayout(layout);

    // make connections
    connect(contentSelectCombo, SIGNAL(currentIndexChanged(int)),
            contentStackedWidget, SLOT(setCurrentIndex(int)));
    connect(treeView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(addNewChart(const QModelIndex &)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)),
            tabWidget, SLOT(removeTab(int)));
}

bool GermanyContentWidget::loadGermanData(const QString &folder)
{
    // load data
    bool ret = germany.loadData(folder);

    // if data load was successfully load the tree model into the view
    if(ret) {
        model = new GermanyDataTreeModel(germany.getCountryData());
        treeView->setModel(model);
        treeView->header()->resizeSection(0, 240);
        treeView->header()->resizeSection(1, 50);
        treeView->header()->resizeSection(2, 60);
    }

    return ret;
}

void GermanyContentWidget::addNewChart(const QModelIndex &index)
{
    if(contentSelectCombo->currentIndex() == 0) {
        QVector<QVariant> rowData = model->rowData(index, Qt::DisplayRole);

        QVector<QDateTime> timestamps;
        CaseData data;

        QString code = "";
        if(!rowData[1].isNull()) {
            code = rowData[1].toString();
        } else if(!rowData[2].isNull()) {
            code = rowData[2].toString();
        }

        if(!germany.getCaseDataByCode(code, data, timestamps)) {
            QMessageBox::information(this, tr("Error"),
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
        tabWidget->addTab(chartWidget, rowData[0].toString());
    }
}
