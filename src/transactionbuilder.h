#ifndef TRANSACTIONBUILDER_H
#define TRANSACTIONBUILDER_H

#include "transaction.h"
#include "annotation.h"

class TransactionBuilder
{
public:
    TransactionBuilder(QStringList tiers = QStringList(), QStringList bd = QStringList());
    virtual ~TransactionBuilder();

    virtual QVector<Transaction> buildTransactions() = 0;

    static AnnotationFile *addFile(QString filename);

    static AnnotationFile *getAFByName(QString filename);
    static void clear();

    QStringList getTiers() const;

protected:
    static QStringList _bdfilenames;
    static QVector<AnnotationFile *> _bd;

    QStringList _curbdfilenames;
    QStringList _tiers;
    QVector<AnnotationFile *> _curbd;


};


class  SimpleTransactionBuilder : public TransactionBuilder
{
public:
    SimpleTransactionBuilder(int step = 5, double overlaystep = 1., QStringList tiers = QStringList(), QStringList bd = QStringList());
    virtual ~SimpleTransactionBuilder() override;

    QVector<Transaction> buildTransactions() override;

private:
    int _step;
    double _overlaystep;
};

class  MainTierTransactionBuilder : public TransactionBuilder
{
public:
    MainTierTransactionBuilder(QString tier = QString("функция"), bool between = false, QStringList tiers = QStringList(), QStringList bd = QStringList());
    virtual ~MainTierTransactionBuilder() override;

    QVector<Transaction> buildTransactions() override;

private:
    QString _tier;
    bool _between;
};

class  TimeOrderTransactionBuilder : public TransactionBuilder
{
public:

    TimeOrderTransactionBuilder(QStringList tiers = QStringList(), QStringList bd = QStringList());
    virtual ~TimeOrderTransactionBuilder() override;

    QVector<Transaction> buildTransactions() override;
};

#endif // TRANSACTIONBUILDER_H
