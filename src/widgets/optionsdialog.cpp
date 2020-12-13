#include "optionsdialog.h"

#include <QDebug>
#include <QRadioButton>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QEvent>
#include <QButtonGroup>

#include "../data/appsettings.h"

OptionsDialog::OptionsDialog(AppSettings *settings, QWidget *parent)
    : QDialog(parent)
    , settings(settings)
{
    initUi();
}

void OptionsDialog::initUi()
{
    resize(200, 150);
    setWindowTitle(tr("Einstellungen"));

    // left layout, with radio buttons
    radioButtonGroup  = new QButtonGroup(this);
    englishRadio = new QRadioButton(tr("Englisch"));
    radioButtonGroup->addButton(englishRadio, OptionsLanguageId::English);
    germanRadio = new QRadioButton(tr("Deutsch"));
    radioButtonGroup->addButton(germanRadio, OptionsLanguageId::German);

    // set selection based on locale
    if(QLocale(QLocale::English, QLocale::UnitedStates).name() == settings->getLanguageCode()) {
        englishRadio->setChecked(true);
    } else if(QLocale(QLocale::German, QLocale::Germany).name() == settings->getLanguageCode()) {
        germanRadio->setChecked(true);
    } else {
        germanRadio->setChecked(true);
    }
    connect(radioButtonGroup, &QButtonGroup::idToggled,
            this, &OptionsDialog::radioChanged);

    // build the layout
    languageLabel = new QLabel(tr("Sprache"));

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(languageLabel);
    leftLayout->addWidget(germanRadio);
    leftLayout->addWidget(englishRadio);
    leftLayout->addStretch();

    // right layout
    closeButton = new QPushButton(tr("Schließen"));
    connect(closeButton, &QPushButton::clicked, this, &OptionsDialog::close);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addStretch();
    rightLayout->addWidget(closeButton);

    // add up all layouts
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);
}

void OptionsDialog::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QDialog::changeEvent(event);
}

void OptionsDialog::retranslateUi()
{
    setWindowTitle(tr("Einstellungen"));
    languageLabel->setText(tr("Sprache"));
    englishRadio->setText(tr("Englisch"));
    germanRadio->setText(tr("Deutsch"));
    closeButton->setText(tr("Schließen"));
}

void OptionsDialog::radioChanged(int id, bool checked)
{
    if(checked) {
        switch(id) {
            case OptionsLanguageId::English:
                settings->setLanguageCode(QLocale(QLocale::English, QLocale::UnitedStates).name());
                break;
            case OptionsLanguageId::German:
                settings->setLanguageCode(QLocale(QLocale::German, QLocale::Germany).name());
                break;
            default:
                break;
        }
    }
}
