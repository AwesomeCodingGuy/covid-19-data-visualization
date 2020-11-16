#include "mapview.h"

#include "../utils/areaitem.h"

#include <QDebug>
#include <QPoint>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsPathItem>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QToolTip>

constexpr qreal zoomFactor = 1.25;

MapView::MapView(QWidget *parent)
    : QGraphicsView(parent)
{
    init();
}

MapView::MapView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    init();
}

void MapView::init()
{
    setMouseTracking(true);
}

void MapView::zoomIn()
{
    scale(zoomFactor, zoomFactor);
}

void MapView::zoomOut()
{
    scale(qreal(1.0f) / zoomFactor, qreal(1.0f) / zoomFactor);
}

bool MapView::event(QEvent *event)
{
    if(event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent*>(event);
        AreaItem *item = qgraphicsitem_cast<AreaItem*>(itemAt(helpEvent->pos()));
        if(item) {
            QToolTip::showText(helpEvent->globalPos(), item->toolTip());
        } else {
            QToolTip::hideText();
            event->ignore();
        }

        return true;
    }

    return QGraphicsView::event(event);
}

void MapView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void MapView::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;

    if(!numDegrees.isNull()) {
        if(numDegrees.y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
    }
}

void MapView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void MapView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void MapView::mouseDoubleClickEvent(QMouseEvent *event)
{
    AreaItem *pathItem = qgraphicsitem_cast<AreaItem*>(itemAt(event->pos()));
    if(pathItem) {
        emit pathItemDoubleClicked(pathItem);
        event->accept();
    }
}
