#ifndef CALLOUT_H
#define CALLOUT_H

#include <QFont>
#include <QTextDocument>
#include <QGraphicsItem>
#include <QAbstractTextDocumentLayout>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QAbstractSeries;
QT_CHARTS_END_NAMESPACE

class Callout : public QGraphicsItem
{
public:
    Callout(QtCharts::QChart *parent, QtCharts::QAbstractSeries *series);

    void setText(const QString &text);
    void setAnchor(QPointF anchorPoint);
    void updateGeometry();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *parent);
    void setSeries(QtCharts::QAbstractSeries *series);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    QTextDocument td;
    QAbstractTextDocumentLayout::PaintContext ctx;
    QString mText;
    QRectF mTextRect;
    QRectF mRect;
    QPointF mAnchor;
    QFont mFont;
    QtCharts::QChart *mChart;
    QtCharts::QAbstractSeries *mSeries;
};

#endif // CALLOUT_H
