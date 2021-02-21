#ifndef VISUALIZATOR_H
#define VISUALIZATOR_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

#include "annotation.h"

class GraphicsTierItem : public QGraphicsRectItem
{
public:
    GraphicsTierItem(int max, QString name, QGraphicsItem *parent = nullptr);

   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
   QString name() const;
   bool active() const;
   void setActive(bool active);

private:
    QString _name;

    bool _active;


};

class GraphicsAnnotationItem : public QGraphicsTextItem
{
public:
    GraphicsAnnotationItem(QString text, int width, QGraphicsItem *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    QPainterPath shape() const;
    QRectF boundingRect() const;
private:
    int _width, t1, t2;
    bool _active;
    QString _text;

};


class Visualizator: public QGraphicsScene
{
    Q_OBJECT
public:
    Visualizator(QObject *parent = nullptr);
    ~Visualizator();

    void mouseMoveEvent(QGraphicsSceneMouseEvent *ev);

    void setmax(int m);
    void setTiers(QStringList &tiers);
    void setAnnotations(QVector<QVector<Annotation *>> &anns);
    void setAnnotationFile(AnnotationFile *af);

    QStringList getActiveTiers();
    void setActiveTiers(QStringList activetiers);

signals:
    void mouseMoved(double x, QString tier);
private:
    QStringList _tierlabels;
    QVector<GraphicsTierItem *> _tiers;
    AnnotationFile *_af;
    QGraphicsLineItem *cursor;
};

#endif // VISUALIZATOR_H
