#include "colorlegend.h"

#include <QEvent>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>

#include "../data/colors.h"

constexpr QSize colorLabelSize{16, 24};

ColorLegend::ColorLegend(QWidget *parent)
    : QWidget(parent)
{
    // add headline
    headlineLabel = new QLabel(tr("7-Tage-Inzidenz"));
    layout.addWidget(headlineLabel, 0, 0, 1, 6, Qt::AlignHCenter);

    // add ranges
    for(int i = 0; i < static_cast<int>(colors::ColorRanges.size()); ++i) {
        // create color label
        auto *colorLabel = new QLabel();
        colorLabel->setMinimumSize(colorLabelSize);
        colorLabel->resize(colorLabelSize);
        colorLabel->setStyleSheet(QString("background-color: %1").arg(colors::ColorRanges[i].color.name()));

        layout.addWidget(colorLabel, i + 1, 0, Qt::AlignLeft);

        // add ranges, with exceptions for the first and the last line
        QLabel *valueLabel = new QLabel(tr("Wert"));
        if(i == 0) {
            layout.addWidget(valueLabel, i + 1, 3, Qt::AlignCenter);
            layout.addWidget(new QLabel("<"), i + 1, 4, Qt::AlignCenter);
            layout.addWidget(new QLabel(QString::number(colors::ColorRanges[i].max)), i + 1, 5, Qt::AlignCenter);
        } else if(i == colors::ColorRanges.size() - 1) {
            layout.addWidget(new QLabel(QString::number(colors::ColorRanges[i].min)), i + 1, 1, Qt::AlignCenter);
            layout.addWidget(new QLabel("<="), i + 1, 2, Qt::AlignCenter);
            layout.addWidget(valueLabel, i + 1, 3, Qt::AlignCenter);
        } else {
            layout.addWidget(new QLabel(QString::number(colors::ColorRanges[i].min)), i + 1, 1, Qt::AlignCenter);
            layout.addWidget(new QLabel("<="), i + 1, 2, Qt::AlignCenter);
            layout.addWidget(valueLabel, i + 1, 3, Qt::AlignCenter);
            layout.addWidget(new QLabel("<"), i + 1, 4, Qt::AlignCenter);
            layout.addWidget(new QLabel(QString::number(colors::ColorRanges[i].max)), i + 1, 5, Qt::AlignCenter);
        }
        valueLabels.append(valueLabel);
    }

    setLayout(&layout);
    setStyleSheet(QString("background-color: %1;").arg(QWidget::palette().color(QPalette::Light).name()));
}

void ColorLegend::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ColorLegend::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

void ColorLegend::retranslateUi()
{
    headlineLabel->setText(tr("7-Tage-Inzidenz"));

    foreach(QLabel *label, valueLabels) {
        label->setText(tr("Wert"));
    }
}
