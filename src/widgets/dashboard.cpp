#include "dashboard.h"
#include "../data/constants.h"
#include "../data/appsettings.h"
#include "../utils/downloadmanager.h"

#include <QGridLayout>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QFileDialog>

Dashboard::Dashboard(AppSettings &settings, QWidget *parent)
    : QWidget(parent)
    , appSettings(settings)
{
    // init downlaod manager
    downloadManager = new DownloadManager(appSettings.downloadFolder, this);

    initWidgets();
}

void Dashboard::initWidgets()
{
    // init layout
    gLayout = new QGridLayout();
    gLayout->setSpacing(10);

    // file layouts
    gGermanyFileLayout = new QGridLayout();
    for(int i = 0; i < 5; ++i) {
        germanyFileInfoLabels[i] = new QLabel();
    }
    gGermanyFileLayout->addWidget(new QLabel(tr("Datei")), 0, 0);
    gGermanyFileLayout->addWidget(new QLabel(tr("Letztes Update")), 0, 1);
    gGermanyFileLayout->addWidget(new QLabel(constants::casesByState), 1, 0);
    gGermanyFileLayout->addWidget(germanyFileInfoLabels[0], 1, 1);
    gGermanyFileLayout->addWidget(new QLabel(constants::casesByAgs), 2, 0);
    gGermanyFileLayout->addWidget(germanyFileInfoLabels[1], 2, 1);
    gGermanyFileLayout->addWidget(new QLabel(constants::deathsByState), 3, 0);
    gGermanyFileLayout->addWidget(germanyFileInfoLabels[2], 3, 1);
    gGermanyFileLayout->addWidget(new QLabel(constants::deathsByAgs), 4, 0);
    gGermanyFileLayout->addWidget(germanyFileInfoLabels[3], 4, 1);
    gGermanyFileLayout->addWidget(new QLabel(constants::ags), 5, 0);
    gGermanyFileLayout->addWidget(germanyFileInfoLabels[4], 5, 1);

    gJhuFileLayout = new QGridLayout();
    for(int i = 0; i < 4; ++i) {
        jhuFileInfoLabels[i] = new QLabel();
    }
    gJhuFileLayout->addWidget(new QLabel(tr("Datei")), 0, 0);
    gJhuFileLayout->addWidget(new QLabel(tr("Letztes Update")), 0, 1);
    gJhuFileLayout->addWidget(new QLabel(constants::casesUS), 1, 0);
    gJhuFileLayout->addWidget(jhuFileInfoLabels[0], 1, 1);
    gJhuFileLayout->addWidget(new QLabel(constants::casesGlobal), 2, 0);
    gJhuFileLayout->addWidget(jhuFileInfoLabels[1], 2, 1);
    gJhuFileLayout->addWidget(new QLabel(constants::deathsUS), 3, 0);
    gJhuFileLayout->addWidget(jhuFileInfoLabels[2], 3, 1);
    gJhuFileLayout->addWidget(new QLabel(constants::deathsGlobal), 4, 0);
    gJhuFileLayout->addWidget(jhuFileInfoLabels[3], 4, 1);

    // init layout
    gLayout->addWidget(new QLabel(tr("Deutschland")), 0, 0, 1, 2);
    germanyRepoUrlLabel = new QLabel(tr("Datenquelle") + QString(": <a href=\"%1\">%1</a>").arg(constants::germanyGithubRepoUrl));
    germanyRepoUrlLabel->setWordWrap(true);
    germanyRepoUrlLabel->setTextFormat(Qt::RichText);
    germanyRepoUrlLabel->setOpenExternalLinks(true);
    germanyRepoUrlLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    gLayout->addWidget(germanyRepoUrlLabel, 1, 0, 1, 4);
    gLayout->addLayout(gGermanyFileLayout, 2, 1, 1, 2);

    gLayout->addWidget(new QLabel(tr("Welt und USA")), 3, 0, 1, 2);
    germanyRepoUrlLabel = new QLabel(tr("Datenquelle") + QString(": <a href=\"%1\">%1</a>").arg(constants::jhuGithubRepoUrl));
    germanyRepoUrlLabel->setWordWrap(true);
    germanyRepoUrlLabel->setTextFormat(Qt::RichText);
    germanyRepoUrlLabel->setOpenExternalLinks(true);
    germanyRepoUrlLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    gLayout->addWidget(germanyRepoUrlLabel, 4, 0, 1, 4);
    gLayout->addLayout(gJhuFileLayout, 5, 1, 1, 2);

    downloadDataButton = new QPushButton(tr("Daten herunterladen / Daten updaten"));
    connect(downloadDataButton, &QPushButton::clicked,
            this, &Dashboard::downloadFiles);
    gLayout->addWidget(downloadDataButton, 6, 1);

    loadDataButton = new QPushButton(tr("Daten laden"));
    connect(loadDataButton, &QPushButton::clicked,
            this, &Dashboard::sigLoadData);
    gLayout->addWidget(loadDataButton, 6, 3);

    gLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding), 7, 4, 1, 1);

    setLayout(gLayout);

    updateFileInfos();
}

QString Dashboard::getFileUpdateString(const QString &filename) const
{
    if(!QFile::exists(appSettings.downloadFolder + filename)) {
        return QString(tr("Datei nicht vorhanden"));
    } else {
        QFileInfo fileInfo(appSettings.downloadFolder + filename);
        return fileInfo.lastModified().toString("dd.MM.yyyy - hh:mm");
    }
}

void Dashboard::updateFileInfos()
{
    germanyFileInfoLabels[0]->setText(getFileUpdateString(constants::casesByState));
    germanyFileInfoLabels[1]->setText(getFileUpdateString(constants::casesByAgs));
    germanyFileInfoLabels[2]->setText(getFileUpdateString(constants::deathsByState));
    germanyFileInfoLabels[3]->setText(getFileUpdateString(constants::deathsByAgs));
    germanyFileInfoLabels[4]->setText(getFileUpdateString(constants::ags));
    jhuFileInfoLabels[0]->setText(getFileUpdateString(constants::casesUS));
    jhuFileInfoLabels[1]->setText(getFileUpdateString(constants::casesGlobal));
    jhuFileInfoLabels[2]->setText(getFileUpdateString(constants::deathsUS));
    jhuFileInfoLabels[3]->setText(getFileUpdateString(constants::deathsGlobal));
}

void Dashboard::downloadFiles()
{
    qDebug() << "Starting download";

    QStringList files = {
        constants::germanyRepoDataUrl + constants::casesByState,
        constants::germanyRepoDataUrl + constants::casesByAgs,
        constants::germanyRepoDataUrl + constants::deathsByState,
        constants::germanyRepoDataUrl + constants::deathsByAgs,
        constants::germanyRepoDataUrl + constants::ags,
        constants::jhuRepoDataUrl + constants::casesUS,
        constants::jhuRepoDataUrl + constants::casesGlobal,
        constants::jhuRepoDataUrl + constants::deathsUS,
        constants::jhuRepoDataUrl + constants::deathsGlobal,
    };

    downloadManager->appendFiles(files);
    downloadManager->startDownload();
    connect(downloadManager, &DownloadManager::downloadsFinished,
            this, &Dashboard::updateFileInfos);
}
