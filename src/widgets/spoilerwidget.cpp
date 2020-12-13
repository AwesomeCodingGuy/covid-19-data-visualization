#include "spoilerwidget.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QPropertyAnimation>

SpoilerWidget::SpoilerWidget(const QString &title,
                             const int animationDuration,
                             QWidget *parent)
    : QWidget(parent)
    , animationDuration(animationDuration)
{
    setStyleSheet(QString("QWidget { background-color: %1;}")
                  .arg(QWidget::palette().color(QPalette::Midlight).name()));

    // configure toolbutton
    toggleButton.setStyleSheet("QToolButton { border: none; }");
    toggleButton.setText(title);
    toggleButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggleButton.setArrowType(Qt::ArrowType::RightArrow);
    toggleButton.setCheckable(true);
    toggleButton.setChecked(false);

    // configure header line
    headerLine.setFrameShape(QFrame::HLine);
    headerLine.setFrameShadow(QFrame::Sunken);
    headerLine.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    // configure content area
    contentArea.setStyleSheet(QString("QScrollArea { border: none; border-top: 1px solid %1; }").arg(QWidget::palette().color(QPalette::Mid).name()));
    contentArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    contentArea.setMaximumHeight(0);
    contentArea.setMinimumHeight(0);

    // configure toogle animation
    toggleAnimation.addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    toggleAnimation.addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    toggleAnimation.addAnimation(new QPropertyAnimation(&contentArea, "maximumHeight"));

    // do not waste any space
    mainLayout.setVerticalSpacing(0);
    mainLayout.setContentsMargins(0, 0, 0, 0);

    // set widgets to layout
    int row = 0;
    mainLayout.addWidget(&toggleButton, row, 0, 1, 1, Qt::AlignLeft);
    //mainLayout.addWidget(&headerLine, row, 2, 1, 1);
    mainLayout.addWidget(&contentArea, ++row, 0, 1, 3);
    setLayout(&mainLayout);

    // connect toggle signal of button
    connect(&toggleButton, &QToolButton::toggled, this, &SpoilerWidget::toggle);
}

void SpoilerWidget::setContentLayout(QLayout &contentLayout)
{
    // delete any previous added layouts and replace with new one
    delete contentArea.layout();
    contentArea.setLayout(&contentLayout);

    // calculate new sizes and set animation properties
    setMinimumWidth(contentLayout.sizeHint().width());
    const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight();
    auto contentHeight = contentLayout.sizeHint().height();
    for(int i = 0; i < toggleAnimation.animationCount() - 1; ++i) {
        QPropertyAnimation *spoilerAnimation = static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(i));
        spoilerAnimation->setDuration(animationDuration);
        spoilerAnimation->setStartValue(collapsedHeight);
        spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
    }
    QPropertyAnimation *contentAnimation = static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(toggleAnimation.animationCount() - 1));
    contentAnimation->setDuration(animationDuration);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);
}

void SpoilerWidget::setCollapsed(bool collapsed)
{
    toggleButton.setChecked(!collapsed);
}

void SpoilerWidget::setButtonName(const QString &title)
{
    toggleButton.setText(title);
}

void SpoilerWidget::toggle(bool collapsed)
{
    // start the toggling animation
    toggleButton.setArrowType(collapsed ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    toggleAnimation.setDirection(collapsed ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    toggleAnimation.start();
}

void SpoilerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
