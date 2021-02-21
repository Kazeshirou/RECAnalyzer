#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QObject>
#include <QVector>
#include <QMap>

class Annotation
{
public:
    Annotation() = default;
    Annotation(QString i, QString txt, int starttime, int endtime);

    static void getAverage();

public:
    QString text;                                  // annotation's string (value)
    QString id;
    long int t1, t2;                               // starttime, endtime (ms)
    static int _n;
    static int _sum;
};


class AnnotationFile : public QObject
{
    Q_OBJECT

public:
    AnnotationFile(QString filename, QObject *parent = nullptr);
    ~AnnotationFile();

private:
    void parse(QString filename);

private slots:
    void headerParsed(QString, QString media);
    void timeOrderParsed(QString, QStringList ids, QVector<int> values);
    void tierParsed(QString, QString tier, QVector<QStringList> annotation, QVector<bool> isalignable);

public:
    QString _filename;
    QStringList _mediafiles;
    QStringList _tiers;
    QMap<QString, long int> _timeorder;
    QVector<QVector<Annotation *>> _annotations;
    QMap<QString, Annotation *> _annotationbyid;
    int _max;

};

#endif // ANNOTATION_H
