#ifndef COLORLEGEND_H
#define COLORLEGEND_H

#include <QWidget>
#include <QGridLayout>

class QLabel;

class ColorLegend : public QWidget
{
    Q_OBJECT
public:
    explicit ColorLegend(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void changeEvent(QEvent *event);

private:
    void retranslateUi();

signals:

private:
    QGridLayout layout;

    QLabel          *headlineLabel;
    QList<QLabel*>  valueLabels;
};

#endif // COLORLEGEND_H
