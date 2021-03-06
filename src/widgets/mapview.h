#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>

class AreaItem;
class CovidDataTreeItem;

class MapView : public QGraphicsView
{
    Q_OBJECT

public:
    MapView(QWidget *parent = nullptr);
    MapView(QGraphicsScene *scene, QWidget *parent = nullptr);

    void init();

private:
    void zoomIn();
    void zoomOut();

protected:
    virtual bool event(QEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void pathItemDoubleClicked(CovidDataTreeItem *);

};

#endif // MAPVIEW_H
