#include "dashboard.h"
#include "../data/appsettings.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QFileDialog>

Dashboard::Dashboard(AppSettings &settings, QWidget *parent)
    : QWidget(parent)
    , appSettings(settings)
{
    initWidgets();
}

void Dashboard::initWidgets()
{
    // init layout
    gLayout = new QGridLayout();
    gLayout->setSpacing(10);
    gLayout->setColumnStretch(0, 2);
    gLayout->setColumnStretch(1, 0);
    gLayout->setColumnStretch(2, 2);

    // init widgets
    germanyDataLabel = new QLabel(tr("Repository von Dr. Jan-Philip Gehrcke"));
    germanyDescriptionLabel = new QLabel(tr("Datenquelle: <br />"
                                            "<a href=\"https://github.com/jgehrcke/covid-19-germany-gae\">"
                                            "https://github.com/jgehrcke/covid-19-germany-gae</a><br /><br />"));
    germanyDescriptionLabel->setWordWrap(true);
    germanyDescriptionLabel->setTextFormat(Qt::RichText);
    germanyDescriptionLabel->setOpenExternalLinks(true);
    germanyDescriptionLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    germanyDataLineEdit = new QLineEdit();
    if(!appSettings.germanyDataPath.isNull()) {
        germanyDataLineEdit->setText(appSettings.germanyDataPath);
    }
    connect(germanyDataLineEdit, &QLineEdit::textChanged,
            this, &Dashboard::germanyFolderChanged);
    germanyDataSearchPathButton = new QPushButton(tr("..."));
    connect(germanyDataSearchPathButton, &QPushButton::clicked,
            this, &Dashboard::selectGermanyFolder);

    jhuDataLabel = new QLabel(tr("Repository der Johns Hopkins Universität"));
    jhuDescriptionLabel  = new QLabel(tr("Datenquelle: <br />"
                                         "<a href=\"https://github.com/CSSEGISandData/COVID-19\">"
                                         "https://github.com/CSSEGISandData/COVID-19</a><br /><br />"));
    jhuDescriptionLabel->setWordWrap(true);
    jhuDescriptionLabel->setTextFormat(Qt::RichText);
    jhuDescriptionLabel->setOpenExternalLinks(true);
    jhuDescriptionLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    jhuDataLineEdit = new QLineEdit();
    if(!appSettings.jhuDataPath.isNull()) {
        jhuDataLineEdit->setText(appSettings.jhuDataPath);
    }
    connect(jhuDataLineEdit, &QLineEdit::textChanged,
            this, &Dashboard::jhuFolderChanged);
    jhuDataSearchPathButton = new QPushButton(tr("..."));
    connect(jhuDataSearchPathButton, &QPushButton::clicked,
            this, &Dashboard::selectJhuFolder);

    infoLabel = new QLabel(tr("Die Dateien können entweder heruntergeladen oder das "
                              "Repository mit git geclont werden. <br />"
                              "Die Daten müssen derzeit täglich händisch aktualisiert werden."));
    infoLabel->setWordWrap(true);
    infoLabel->setTextFormat(Qt::RichText);

    loadDataButton = new QPushButton(tr("Daten laden"));

    // build the layout
    gLayout->addWidget(germanyDataLabel, 0, 0);
    gLayout->addWidget(germanyDescriptionLabel, 0, 2, 3, 1);
    gLayout->addWidget(germanyDataLineEdit, 1, 0);
    gLayout->addWidget(germanyDataSearchPathButton, 1, 1);

    gLayout->addWidget(jhuDataLabel, 3, 0);
    gLayout->addWidget(jhuDescriptionLabel, 3, 2, 3, 1);
    gLayout->addWidget(jhuDataLineEdit, 4, 0);
    gLayout->addWidget(jhuDataSearchPathButton, 4, 1);

    gLayout->addWidget(infoLabel, 6, 2);
    gLayout->addWidget(loadDataButton, 7, 2);
    gLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding), 8, 0, 1, 3);

    setLayout(gLayout);
}

void Dashboard::selectGermanyFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Ordner öffnen"),
                                                    "C:\\",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    germanyDataLineEdit->setText(dir);
}

void Dashboard::selectJhuFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Ordner öffnen"),
                                                    "C:\\",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    jhuDataLineEdit->setText(dir);
}

void Dashboard::germanyFolderChanged(const QString &text)
{
    appSettings.germanyDataPath = text;
}

void Dashboard::jhuFolderChanged(const QString &text)
{
    appSettings.jhuDataPath = text;
}
