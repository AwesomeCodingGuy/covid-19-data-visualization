#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>

class AreaItem;

class MapView : public QGraphicsView
{
    Q_OBJECT;

public:
    MapView(QWidget *parent = nullptr);
    MapView(QGraphicsScene *scene, QWidget *parent = nullptr);

    void init();

private:
    void zoomIn();
    void zoomOut();

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void pathItemDoubleClicked(AreaItem *);

};

#endif // MAPVIEW_H
