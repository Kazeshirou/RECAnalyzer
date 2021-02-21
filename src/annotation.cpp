#include "annotation.h"

#include "xmlparser.h"

#include <QDebug>

AnnotationFile::AnnotationFile(QString filename, QObject *parent):
    QObject(parent),
    _max(0)
{
    parse(filename);
}

AnnotationFile::~AnnotationFile()
{
    for (auto t : _annotations)
        for (auto a : t)
            delete a;
}



void AnnotationFile::parse(QString filename)
{
    _filename = filename;

    XmlParser parser;
    connect(&parser, &XmlParser::fileHederHasParsed, this, &AnnotationFile::headerParsed);
    connect(&parser, &XmlParser::timeOrderHasParsed, this, &AnnotationFile::timeOrderParsed);
    connect(&parser, &XmlParser::tierAnnotationHasParsed, this, &AnnotationFile::tierParsed);

    parser.parseAnnotation(filename);

    disconnect(&parser, &XmlParser::fileHederHasParsed, this, &AnnotationFile::headerParsed);
    disconnect(&parser, &XmlParser::timeOrderHasParsed, this, &AnnotationFile::timeOrderParsed);
    disconnect(&parser, &XmlParser::tierAnnotationHasParsed, this, &AnnotationFile::tierParsed);
}


void AnnotationFile::headerParsed(QString, QString media)
{
    _mediafiles.append(media);
}

void AnnotationFile::timeOrderParsed(QString, QStringList ids, QVector<int> values)
{
    for (int i = 0; i < ids.length(); i++) {
        _timeorder.insert(ids[i], values[i]);
        if (values[i] > _max)
            _max = values[i];
    }

}

void AnnotationFile::tierParsed(QString, QString tier, QVector<QStringList> annotation, QVector<bool> isalignable)
{
    int it = _tiers.indexOf(tier);
    if (it < 0) {
        _tiers.append(tier);
        _annotations.append(QVector<Annotation *>());
        it = _tiers.length()-1;
    }


    for (int i = 0; i < annotation.length(); i++) {
        if (isalignable[i]) {
            _annotations[it].append(new Annotation(annotation[i][0], annotation[i][1], _timeorder.value(annotation[i][2], 0), _timeorder.value(annotation[i][3], 0)));
            _annotationbyid.insert(annotation[i][0], _annotations[it].last());
        }
        else {
            Annotation *p = _annotationbyid.value(annotation[i][2], nullptr);
            if (p) {
                _annotations[it].append(new Annotation(annotation[i][0], annotation[i][1], p->t1, p->t2));
                _annotationbyid.insert(annotation[i][0], _annotations[it].last());
            }
            else
                qDebug() << tier << annotation[i][0] << annotation[i][1];

        }
    }
}


int Annotation::_sum = 0;
int Annotation::_n = 0;

Annotation::Annotation(QString i, QString txt, int starttime, int endtime) :
    text(txt),
    id(i)
{
    t1 = starttime;
    t2 = endtime;
    _sum += endtime-starttime;
    _n++;
}

void Annotation::getAverage()
{
    qDebug() << _sum/double(_n);
}
