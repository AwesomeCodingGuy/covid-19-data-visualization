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

signals:

private:
    QGridLayout layout;

};

#endif // COLORLEGEND_H
