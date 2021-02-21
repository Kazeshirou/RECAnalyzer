#include "visualizator.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QMessageBox>
#include <QDebug>

#define HEIGHT 50

Visualizator::Visualizator(QObject *parent) :
    QGraphicsScene(parent),
    _af(nullptr),
    cursor(nullptr)
{

}

Visualizator::~Visualizator()
{
    for (auto t : _tiers)
        delete t;
    delete cursor;
}

void Visualizator::mouseMoveEvent(QGraphicsSceneMouseEvent *ev)
{

    if (ev->scenePos().x() > 0) {
        cursor->setPos(ev->scenePos().x(), 0);
        cursor->show();
        int t = static_cast<int>(ev->scenePos().y()/HEIGHT);
        QString tier("");
        if ((t >= 0) && (t < _af->_tiers.length()))
            tier = _af->_tiers[t];
        emit mouseMoved(static_cast<double>(ev->scenePos().x()), tier);
    }
    else {
        cursor->hide();
    }
}



void Visualizator::setTiers(QStringList &tiers)
{
    _tierlabels = tiers;
    if (cursor)
        delete cursor;
    cursor = new QGraphicsLineItem;
    addItem(cursor);
    cursor->setPen(QPen(QColor(117, 14, 243, 225)));
    cursor->hide();
    cursor->setLine(0, 0, 0, tiers.length()*HEIGHT);


    _tiers.clear();

    for (int i = 0; i < tiers.length(); i++) {
        _tiers.append(new GraphicsTierItem(_af->_max, tiers[i]));
        addItem(_tiers[i]);
        _tiers[i]->moveBy(0, i*HEIGHT);
        QGraphicsTextItem *t = addText(tiers[i]);
        t->setFont(QFont("Times", 10));
        t->moveBy(-150, i*HEIGHT);
        t->setTextWidth(150);
    }
}

void Visualizator::setAnnotations(QVector<QVector<Annotation *> > &anns)
{

    for (int i = 0; i < anns.length(); i++)
        for (auto a : anns[i]) {
            GraphicsAnnotationItem *item = new GraphicsAnnotationItem(a->id + " " + a->text, a->t2 - a->t1, _tiers[i]);
            item->moveBy(a->t1, 0);
        }
}

void Visualizator::setAnnotationFile(AnnotationFile *af)
{
    clear();
    cursor = nullptr;
    _af = af;

    setSceneRect(-150, 0, _af->_max+105, _af->_tiers.length()*HEIGHT+5);
    setTiers(_af->_tiers);
    setAnnotations(_af->_annotations);

}

QStringList Visualizator::getActiveTiers()
{
    QStringList tiers;
    for (auto t : _tiers)
        if (t->active())
            tiers << t->name();
    return tiers;
}

void Visualizator::setActiveTiers(QStringList activetiers)
{
    for (auto t : _tiers)
        if (activetiers.contains(t->name()))
            t->setActive(true);
}

GraphicsTierItem::GraphicsTierItem(int max, QString name, QGraphicsItem *parent) :
    QGraphicsRectItem(parent),
    _name(name),
    _active(true)
{
    setRect(0, 0, max, HEIGHT);
    setPen(QPen(QColor(117, 14, 243, 225)));
    setBrush(QBrush(QColor(200, 222, 254, 225), Qt::SolidPattern));

}

void GraphicsTierItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *)
{
    _active = !_active;
    if (_active)
        setBrush(QBrush(QColor(200, 222, 254, 225), Qt::SolidPattern));
    else
        setBrush(QBrush(QColor(200, 222, 254, 125), Qt::SolidPattern));
}

bool GraphicsTierItem::active() const
{
    return _active;
}

void GraphicsTierItem::setActive(bool active)
{
    _active = active;
}

QString GraphicsTierItem::name() const
{
    return _name;
}

GraphicsAnnotationItem::GraphicsAnnotationItem(QString text, int width, QGraphicsItem *parent) :
    QGraphicsTextItem(text, parent),
    _width(width),
    _active(false)
{
    setTextWidth(width);
    setFont(QFont("Times", 20));

    setToolTip(text);
}

void GraphicsAnnotationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen p = painter->pen();
    QBrush b = painter->brush();
    painter->setPen(QPen(QColor(130, 14, 100, 225)));
    if (_active)
        painter->setBrush(QBrush(QColor(130, 14, 100, 225), Qt::SolidPattern));
    else {
        painter->setBrush(QBrush(QColor(130, 14, 100, 80), Qt::SolidPattern));

    }
    painter->drawRect(boundingRect());
    painter->setPen(p);
    painter->setBrush(b);
    QGraphicsTextItem::paint(painter, option, widget);
}

QPainterPath GraphicsAnnotationItem::shape() const
{
    QPainterPath path;
    QRectF r = boundingRect();
    path.addRect(r);
    return path;
}

QRectF GraphicsAnnotationItem::boundingRect() const
{
    QRectF r = QGraphicsTextItem::boundingRect();
    r.setHeight(HEIGHT);
    return r;
}


