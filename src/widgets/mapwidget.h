#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QMap>
#include <QWidget>
#include <QGraphicsScene>

class QComboBox;

class MapView;
class SpoilerWidget;

class CovidDataTreeItem;

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

    bool loadMapData(const CovidDataTreeItem &rootItem);
    void addScene(const CovidDataTreeItem &sceneItem);

    void recursiveAddGraphicsItemsToScene(QGraphicsScene *scene, const CovidDataTreeItem &sceneItem);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void changeEvent(QEvent *event);

private:
    void adjustSceneRect(QGraphicsScene *scene, int value);
    void initUi();
    void resetSceneMap();

    void retranslateUi();

public slots:
    void sceneSelectComboChanged(const QString &text);

signals:
    void pathItemDoubleClicked(CovidDataTreeItem *);

private:
    MapView *mapView;

    QMap<QString, QGraphicsScene*> scenes;

    QComboBox *sceneSelectCombo;
    SpoilerWidget *colorLegend;
};

#endif // MAPWIDGET_H
