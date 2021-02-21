#ifndef DOUBLETABLEWIDGETITEM_H
#define DOUBLETABLEWIDGETITEM_H

#include <QTableWidgetItem>

class DoubleTableWidgetItem : public QTableWidgetItem
{
public:
    DoubleTableWidgetItem() :
        QTableWidgetItem (1000)
    {

    }

    DoubleTableWidgetItem(QString t) :
        QTableWidgetItem(t, 1000)
    {

    }

    ~DoubleTableWidgetItem() = default;

    bool operator<(const QTableWidgetItem &other) const {
        return this->text().toDouble() > other.text().toDouble();
    }

};

#endif // DOUBLETABLEWIDGETITEM_H
