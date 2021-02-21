#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QObject>
#include <QXmlStreamReader>


class XmlParser : public QObject
{
    Q_OBJECT
public:
    XmlParser();

    void parseTemplate(QString filename);
    void parseAnnotation(QString filename);

private:
    void parseTier(QXmlStreamReader &xml, QVector<QStringList> &tiers);
    void parseLinguistictype(QXmlStreamReader &xml, QVector<QStringList> &linguistictypes);
    void parseConstraint(QXmlStreamReader &xml, QVector<QStringList> &constraints);
    void parseFileHeder(QXmlStreamReader &xml, QString &media);
    void parseTimeOrder(QXmlStreamReader &xml, QStringList &timenames, QVector<int> &timevalues);
    void parseTierAnnotation(QXmlStreamReader &xml, QString &tierid, QVector<QStringList> &tier, QVector<bool> &isalignable);

signals:
    void templateHasParsed(QVector<QStringList> tiers, QVector<QStringList> linguistictypes, QVector<QStringList> constraints);
    void fileHederHasParsed(QString filename, QString mediafilename);
    void timeOrderHasParsed(QString filename, QStringList timeslotname, QVector<int> value);
    void tierAnnotationHasParsed(QString filename, QString tier, QVector<QStringList> annotation, QVector<bool> isalignable);


public slots:

};

#endif // XMLPARSER_H
