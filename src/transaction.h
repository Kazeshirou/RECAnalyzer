#ifndef CONAPRIORIDATA_H
#define CONAPRIORIDATA_H

#include <QVector>
#include <QStringList>
#include <QBitArray>
#include <QDebug>
#include <QTextStream>
#include <QDataStream>

class Transaction
{
public:
    Transaction();
    Transaction(QStringList items);
    Transaction(QBitArray bits, int sup = 0);
    ~Transaction() = default;

    void print(bool on = false);
    QTextStream &print(QTextStream &d, bool on = false);
    QDataStream &print(QDataStream &d, bool on = false);


    Transaction operator~();
    Transaction operator&(Transaction &t);
    Transaction operator|(Transaction &t);
    Transaction operator^(Transaction &t);


    bool operator==(Transaction &t);
    bool operator!=(Transaction &t);

    int count(bool on);
    int size();

    QBitArray bits() const;

    QVector<Transaction *> _contains;
    static Transaction getFullTransaction();
    static QStringList _allitems;

private:
    QBitArray _bits;
    int _sup;
};

#endif // CONAPRIORIDATA_H
