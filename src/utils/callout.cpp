#include "callout.h"

#include <QPainter>
#include <QDebug>
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QtCharts/QChart>

Callout::Callout(QtCharts::QChart *parent, QtCharts::QAbstractSeries *series)
    : QGraphicsItem(parent)
    , mChart(parent)
    , mSeries(series)
{

}

void Callout::setText(const QString &newText)
{
    td.setHtml(newText);
    ctx.clip = QRect(0, 0, td.size().width(), td.size().height());
    ctx.clip.translate(5, 5);

    prepareGeometryChange();
    mRect = ctx.clip.adjusted(-5, -5, 5, 5);
}

void Callout::setAnchor(QPointF anchorPoint)
{
    mAnchor = anchorPoint;
}

void Callout::updateGeometry()
{
    prepareGeometryChange();

    QSizeF windowSize = mChart->size();
    QPointF anchor = mChart->mapToPosition(mAnchor);
    QPoint offset(10, -50);
    if(anchor.x() + 10 + mRect.width() > windowSize.width()) {
        offset.setX(-mRect.width() - 10);
    }
    if(anchor.y() - 50 < 0) {
        offset.setY(10 + mRect.height());
    }

    setPos(anchor + offset);
}

QRectF Callout::boundingRect() const
{
    QPointF anchor = mapFromParent(mChart->mapToPosition(mAnchor));
    QRectF rect;
    rect.setLeft(qMin(mRect.left(), anchor.x()));
    rect.setRight(qMax(mRect.right(), anchor.x()));
    rect.setTop(qMin(mRect.top(), anchor.y()));
    rect.setBottom(qMax(mRect.bottom(), anchor.y()));
    return rect;
}

void Callout::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *parent)
{
    Q_UNUSED(option);
    Q_UNUSED(parent);
    QPainterPath path;
    path.addRoundedRect(mRect, 5, 5);

    QPointF anchor = mapFromParent(mChart->mapToPosition(mAnchor));
    if(!mRect.contains(anchor)) {
        QPointF p1;
        QPointF p2;

        bool above = anchor.y() <= mRect.top();
        bool aboveCenter = anchor.y() > mRect.top() && anchor.y() <= mRect.center().y();
        bool belowCenter = anchor.y() > mRect.center().y() && anchor.y() <= mRect.bottom();
        bool below = anchor.y() > mRect.bottom();

        bool onLeft = anchor.x() <= mRect.left();
        bool leftOfCenter = anchor.x() > mRect.left() && anchor.x() <= mRect.center().x();
        bool rightOfCenter = anchor.x() > mRect.center().x() && anchor.x() <= mRect.right();
        bool onRight = anchor.x() > mRect.right();

        qreal x = (onRight + rightOfCenter) * mRect.width();
        qreal y = (below + belowCenter) * mRect.height();
        bool cornerCase = (above && onLeft) || (above && onRight) || (below && onLeft) || (below && onRight);
        bool vertical = qAbs(anchor.x() - x) > qAbs(anchor.y() - y);

        qreal x1 = x + leftOfCenter * 10 - rightOfCenter * 20 + cornerCase * !vertical * (onLeft * 10 - onRight * 20);
        qreal y1 = y + aboveCenter * 10 - belowCenter * 20 + cornerCase * vertical * (above * 10 - below * 20);
        p1.setX(x1);
        p1.setY(y1);

        qreal x2 = x + leftOfCenter * 20 - rightOfCenter * 10 + cornerCase * !vertical * (onLeft * 20 - onRight * 10);
        qreal y2 = y + aboveCenter * 20 - belowCenter * 10 + cornerCase * vertical * (above * 20 - below * 10);
        p2.setX(x2);
        p2.setY(y2);

        path.moveTo(p1);
        path.lineTo(anchor);
        path.lineTo(p2);
        path = path.simplified();
    }

    painter->setBrush(QColor(255, 255, 255));
    painter->drawPath(path);
    painter->translate(5, 5);
    td.documentLayout()->draw(painter, ctx);
}

void Callout::setSeries(QtCharts::QAbstractSeries *series)
{
    mSeries = series;
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
