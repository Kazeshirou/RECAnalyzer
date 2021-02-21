#include "transactionbuilder.h"

//#include <QFile>

QStringList TransactionBuilder::_bdfilenames;
QVector<AnnotationFile *> TransactionBuilder::_bd;

TransactionBuilder::TransactionBuilder(QStringList tiers, QStringList bd) :
    _curbdfilenames(bd),
    _tiers(tiers)
{
    for (auto file : bd) {
        AnnotationFile *a = addFile(file);
        if (a)
            _curbd.append(a);
        else {
            _curbd.append(_bd[_bdfilenames.indexOf(file)]);
        }
    }
}

TransactionBuilder::~TransactionBuilder()
{

}

AnnotationFile *TransactionBuilder::addFile(QString filename)
{
    int i = _bdfilenames.indexOf(filename);
    if (i >= 0)
        return nullptr;
    else {
        _bdfilenames.append(filename);
        _bd.append(new AnnotationFile(filename));

//        QFile stat("tiers.txt");
//        stat.open(QIODevice::WriteOnly);
//        QTextStream statstream(&stat);

//        for (auto tier : _bd.last()->_tiers) {
//            statstream << "<< \"" << tier << "\" ";
//        }
//        statstream << ";\n";
//        stat.close();
        return _bd.last();
    }
}



AnnotationFile *TransactionBuilder::getAFByName(QString filename)
{
    int i = _bdfilenames.indexOf(filename);
    if (i >= 0)
        return _bd[i];
    else
        return nullptr;
}

void TransactionBuilder::clear()
{
    for (auto a : _bd)
        delete a;
}

QStringList TransactionBuilder::getTiers() const
{
    return _tiers;
}

SimpleTransactionBuilder::SimpleTransactionBuilder(int step, double overlaystep, QStringList tiers, QStringList bd) :
    TransactionBuilder(tiers, bd),
    _step(step),
    _overlaystep(overlaystep)

{
    if (_overlaystep < 0.01)
        _overlaystep = 0.01;
}

SimpleTransactionBuilder::~SimpleTransactionBuilder()
{

}

QVector<Transaction> SimpleTransactionBuilder::buildTransactions()
{
    QVector<Transaction> transactions;

    for (auto file : _curbd) {
        QVector<int> indexes(file->_annotations.length(), 0);
        int t1 = 0, t2 = _step;
        while (t1 < file->_max) {
            QStringList annotations;
            for (int i = 0; i < file->_annotations.length(); i++) {
                if (!_tiers.contains(file->_tiers[i]))
                    continue;
                if (indexes[i] >= file->_annotations[i].length())
                    continue;
                Annotation *a = file->_annotations[i][indexes[i]];
                while ((a->t2 <= t1) && (++indexes[i] < file->_annotations[i].length()))
                    a = file->_annotations[i][indexes[i]];
                if (indexes[i] >= file->_annotations[i].length())
                    continue;
                int oi = indexes[i];
                while ((a->t1 < t2) && (++indexes[i] < file->_annotations[i].length())) {
                    annotations.append(QString("%1_%2").arg(file->_tiers[i]).arg(a->text));
                    a = file->_annotations[i][indexes[i]];
                }
                indexes[i] = oi;
            }
            Transaction t(annotations);
            if (t.count(true))
                transactions.append(Transaction(annotations));

            int delta = static_cast<int>(_step*_overlaystep);
            t1 += delta;
            t2 += delta;
        }
    }

    return transactions;
}

MainTierTransactionBuilder::MainTierTransactionBuilder(QString tier, bool between, QStringList tiers, QStringList bd) :
    TransactionBuilder(tiers, bd),
    _tier(tier),
    _between(between)
{

}

MainTierTransactionBuilder::~MainTierTransactionBuilder()
{

}

QVector<Transaction> MainTierTransactionBuilder::buildTransactions()
{
    QVector<Transaction> transactions;

    for (auto file : _curbd) {
        int maintier = file->_tiers.indexOf(_tier);
//        if (maintier < 0)
//            continue;
//        if (!file->_annotations[maintier].length())
//            continue;
        QVector<int> indexes(file->_annotations.length(), 0);
        int t1 = 0, t2 = 0;
        while ((maintier >= 0) && (indexes[maintier] < file->_annotations[maintier].length())) {
            Annotation *maina = file->_annotations[maintier][indexes[maintier]];
            if (_between) {
                t1 = t2;
                t2 = maina->t1;
                QStringList annotations;
                for (int i = 0; i < file->_annotations.length(); i++) {
                    if (!_tiers.contains(file->_tiers[i]))
                        continue;
                    if (i == maintier)
                        continue;
                    if (indexes[i] >= file->_annotations[i].length())
                        continue;
                    Annotation *a = file->_annotations[i][indexes[i]];
                    while ((a->t2 <= t1) && (++indexes[i] < file->_annotations[i].length()))
                        a = file->_annotations[i][indexes[i]];
                    if (indexes[i] >= file->_annotations[i].length())
                        continue;
                    int oi = indexes[i];
                    while ((a->t1 < t2) && (++indexes[i] < file->_annotations[i].length())) {
                        annotations.append(QString("%1_%2").arg(file->_tiers[i]).arg(a->text));
                        a = file->_annotations[i][indexes[i]];
                    }
                    indexes[i] = oi;
                }
                transactions.append(Transaction(annotations));
            }
            t1 = t2;
            t2 = maina->t2;
            QStringList annotations;
            annotations.append(QString("%1_%2").arg(_tier).arg(maina->text));
            for (int i = 0; i < file->_annotations.length(); i++) {
                if (!_tiers.contains(file->_tiers[i]))
                    continue;
                if (i == maintier)
                    continue;
                if (indexes[i] >= file->_annotations[i].length())
                    continue;
                Annotation *a = file->_annotations[i][indexes[i]];
                while ((a->t2 <= t1) && (++indexes[i] < file->_annotations[i].length()))
                    a = file->_annotations[i][indexes[i]];
                if (indexes[i] >= file->_annotations[i].length())
                    continue;
                int oi = indexes[i];
                while ((a->t1 < t2) && (++indexes[i] < file->_annotations[i].length())) {
                    annotations.append(QString("%1_%2").arg(file->_tiers[i]).arg(a->text));
                    a = file->_annotations[i][indexes[i]];
                }
                indexes[i] = oi;
            }
            transactions.append(Transaction(annotations));

            indexes[maintier]++;
        }

        if (_between) {
            t1 = t2;
            t2 = file->_max;
            QStringList annotations;
            for (int i = 0; i < file->_annotations.length(); i++) {
                if (!_tiers.contains(file->_tiers[i]))
                    continue;
                if (i == maintier)
                    continue;
                if (indexes[i] >= file->_annotations[i].length())
                    continue;
                Annotation *a = file->_annotations[i][indexes[i]];
                while ((a->t2 <= t1) && (++indexes[i] < file->_annotations[i].length()))
                    a = file->_annotations[i][indexes[i]];
                if (indexes[i] >= file->_annotations[i].length())
                    continue;
                int oi = indexes[i];
                while ((a->t1 < t2) && (++indexes[i] < file->_annotations[i].length())) {
                    annotations.append(QString("%1_%2").arg(file->_tiers[i]).arg(a->text));
                    a = file->_annotations[i][indexes[i]];
                }
                indexes[i] = oi;
            }
            transactions.append(Transaction(annotations));
        }
    }

    return transactions;
}


TimeOrderTransactionBuilder::TimeOrderTransactionBuilder(QStringList tiers, QStringList bd) :
    TransactionBuilder(tiers, bd)
{

}

TimeOrderTransactionBuilder::~TimeOrderTransactionBuilder()
{

}

QVector<Transaction> TimeOrderTransactionBuilder::buildTransactions()
{
    QVector<Transaction> transactions;

    for (auto file : _curbd) {
        int index = 0;
        auto timeorder = file->_timeorder.values();
        if (timeorder.length() < 2)
            continue;
        std::sort(timeorder.begin(), timeorder.end());
        QVector<int> indexes(file->_annotations.length(), 0);
        int t1 = 0, t2 = timeorder[index];
        while (++index < timeorder.length()) {
            t1 = t2;
            t2 = timeorder[index];
            QStringList annotations;
            for (int i = 0; i < file->_annotations.length(); i++) {
                if (!_tiers.contains(file->_tiers[i]))
                    continue;
                if (indexes[i] >= file->_annotations[i].length())
                    continue;
                Annotation *a = file->_annotations[i][indexes[i]];
                while ((a->t2 <= t1) && (++indexes[i] < file->_annotations[i].length()))
                    a = file->_annotations[i][indexes[i]];
                if (indexes[i] >= file->_annotations[i].length())
                    continue;
                int oi = indexes[i];
                while ((a->t1 < t2) && (++indexes[i] < file->_annotations[i].length())) {
                    annotations.append(QString("%1_%2").arg(file->_tiers[i]).arg(a->text));
                    a = file->_annotations[i][indexes[i]];
                }
                indexes[i] = oi;
            }
            transactions.append(Transaction(annotations));
        }
    }

    return transactions;
}
