#ifndef SPOILERWIDGET_H
#define SPOILERWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QToolButton>
#include <QFrame>
#include <QParallelAnimationGroup>
#include <QScrollArea>

class SpoilerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpoilerWidget(const QString &title,
                           const int animationDuration = 300,
                           QWidget *parent = nullptr);

    void setContentLayout(QLayout &contentLayout);
    void setCollapsed(bool collapsed);

    void setButtonName(const QString &title);

public:
    void toggle(bool collapsed);

protected:
    virtual void paintEvent(QPaintEvent *event);

signals:

private:
    QGridLayout mainLayout;
    QToolButton toggleButton;
    QFrame headerLine;
    QParallelAnimationGroup toggleAnimation;
    QScrollArea contentArea;
    int animationDuration;
};

#endif // SPOILERWIDGET_H
