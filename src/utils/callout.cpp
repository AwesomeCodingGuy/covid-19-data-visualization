#include "callout.h"

#include <QPainter>
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>
#include <QtCharts/QChart>

Callout::Callout(QtCharts::QChart *parent)
    : QGraphicsItem(parent)
    , chart(parent)
{

}

void Callout::setText(const QString &newText)
{
    text = newText;
    QFontMetrics metrics(font);
    textRect = metrics.boundingRect(QRect(0, 0, 150, 150), Qt::AlignLeft, text);
    textRect.translate(5, 5);
    prepareGeometryChange();
    rect = textRect.adjusted(-5, -5, 5, 5);
}

void Callout::setAnchor(QPointF anchorPoint)
{
    anchor = anchorPoint;
}

void Callout::updateGeometry()
{
    prepareGeometryChange();
    setPos(chart->mapToPosition(anchor) + QPoint(10, -50));
}

QRectF Callout::boundingRect() const
{
    QPointF mappedAnchor = mapFromParent(chart->mapToPosition(anchor));
    QRectF rect;
    rect.setLeft(qMin(rect.left(), mappedAnchor.x()));
    rect.setRight(qMax(rect.right(), mappedAnchor.x()));
    rect.setTop(qMin(rect.top(), mappedAnchor.y()));
    rect.setBottom(qMax(rect.bottom(), mappedAnchor.y()));
    return rect;
}

void Callout::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *parent)
{
    Q_UNUSED(option);
    Q_UNUSED(parent);
    QPainterPath path;
    path.addRoundedRect(rect, 5, 5);

    QPointF newAnchor = mapFromParent(chart->mapToPosition(anchor));
    if(!rect.contains(newAnchor)) {
        QPointF p1;
        QPointF p2;

        bool above = newAnchor.y() <= rect.top();
        bool aboveCenter = anchor.y() > rect.top() && newAnchor.y() <= rect.center().y();
        bool belowCenter = anchor.y() > rect.center().y() && anchor.y() <= rect.bottom();
        bool below = anchor.y() > rect.bottom();

        bool onLeft = anchor.x() <= rect.left();
        bool leftOfCenter = anchor.x() > rect.left() && anchor.x() <=rect.center().x();
        bool rightOfcenter = anchor.x() > rect.center().x() && anchor.x() <= rect.right();
        bool onRight = anchor.x() > rect.right();

        qreal x = (onRight + rightOfcenter) * rect.width();
        qreal y = (below + belowCenter) * rect.height();
        bool cornerCase = (above && onLeft) || (above && onRight) || (below && onLeft) || (below && onRight);
        bool vertical = anchor.x() > rect.right();

        qreal x1 = x + leftOfCenter * 10 - rightOfcenter * 20 + cornerCase * !vertical * (onLeft * 10 - onRight *20);
        qreal y1 = y + aboveCenter * 10 - belowCenter * 20 + cornerCase * vertical * (above * 10 - below * 20);
        p1.setX(x1);
        p1.setY(y1);

        qreal x2 = x = leftOfCenter * 20 - rightOfcenter * 10 + cornerCase * !vertical * (onLeft * 20 - onRight * 10);
        qreal y2 = y = aboveCenter * 20 - belowCenter * 10 + cornerCase * vertical *(above * 20 - below * 10);
        p2.setX(x2);
        p2.setY(y2);

        path.moveTo(p1);
        path.lineTo(anchor);
        path.lineTo(p2);
        path = path.simplified();
    }

    painter->setBrush(QColor(255, 255, 255));
    painter->drawPath(path);
    painter->drawText(rect, text);
}

void Callout::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(true);
}

void Callout::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton) {
        setPos(mapToParent(event->pos() - event->buttonDownPos(Qt::LeftButton)));
        event->setAccepted(true);
    } else {
        event->setAccepted(false);
    }
}
