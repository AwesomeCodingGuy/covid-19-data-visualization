#ifndef CALLOUT_H
#define CALLOUT_H

#include <QFont>
#include <QGraphicsItem>

namespace QtCharts {
class QChart;
}

class Callout : public QGraphicsItem
{
public:
    Callout(QtCharts::QChart *parent);

    void setText(const QString &text);
    void setAnchor(QPointF anchor);
    void updateGeometry();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *parent);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    QString text;
    QRectF textRect;
    QRectF rect;
    QPointF anchor;
    QFont font;
    QtCharts::QChart *chart;
};

#endif // CALLOUT_H
