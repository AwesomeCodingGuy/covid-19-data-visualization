#ifndef GERMANYMAPWIDGET_H
#define GERMANYMAPWIDGET_H

#include <QWidget>

class GermanyMapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GermanyMapWidget(QWidget *parent = nullptr);

    bool loadGermanMap(const QString &file);

signals:

};

#endif // GERMANYMAPWIDGET_H
