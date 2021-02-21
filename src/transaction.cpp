#include "transaction.h"

#include <QDebug>


QStringList Transaction::_allitems;

Transaction::Transaction() :
    _bits(_allitems.length()),
    _sup(0)
{

}

Transaction::Transaction(QStringList items) :
    _bits(_allitems.length()),
    _sup(0)
{
    for (auto s : items) {
        s.replace(" ", "_");
        if (!_allitems.contains(s)) {
            _allitems.append(s);
            _bits.resize(_allitems.length());
        }
        _bits.setBit(_allitems.indexOf(s));
    }
}

Transaction::Transaction(QBitArray bits, int sup) :
    _bits(bits),
    _sup(sup)
{
    if (_bits.size() != _allitems.size())
        _bits.resize(_allitems.size());
}



void Transaction::print(bool on)
{
    if (!_bits.count(true))
        return;
    QString s;
    for (int i = 0; i < _bits.size(); i++)
        if (_bits.at(i))
            s += _allitems[i] + " ";
    if (on)
        s += QString::number(_sup);
    else
        s.resize(s.length()-1);
    qDebug() << s;
}

QTextStream &Transaction::print(QTextStream &d, bool on)
{
    if (!_bits.count(true))
        return d;
    QString s;
    for (int i = 0; i < _bits.size(); i++)
        if (_bits.at(i))
            s += _allitems[i] + " ";
    if (on)
        s += QString::number(_sup);
    else
        s.resize(s.length()-1);
    d << s;
    d << "\n";
    return d;
}

QDataStream &Transaction::print(QDataStream &d, bool on)
{
    if (!_bits.count(true))
        return d;
    QString s;
    for (int i = 0; i < _bits.size(); i++)
        if (_bits.at(i))
            s += _allitems[i] + " ";
    if (on)
        s += QString::number(_sup);
    else
        s.resize(s.length()-1);
    d << s;
    d << "\n";
    return d;
}

Transaction Transaction::operator~()
{
    return Transaction(~_bits);
}

Transaction Transaction::operator&(Transaction &t)
{
    return Transaction(_bits&t._bits);
}

Transaction Transaction::operator|(Transaction &t)
{
    return Transaction(_bits|t._bits);
}

Transaction Transaction::operator^(Transaction &t)
{
    return Transaction(_bits^t._bits);
}

bool Transaction::operator==(Transaction &t)
{
    return _bits == t._bits;
}

bool Transaction::operator!=(Transaction &t)
{
    return _bits != t._bits;
}

int Transaction::count(bool on)
{
    return _bits.count(on);
}

int Transaction::size()
{
    return _bits.size();
}

QBitArray Transaction::bits() const
{
    return _bits;
}

Transaction Transaction::getFullTransaction()
{
    return Transaction(_allitems);
}
