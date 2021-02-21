#include "xmlparser.h"

#include <QFile>
#include <QDebug>

XmlParser::XmlParser()
{

}

void XmlParser::parseTemplate(QString filename)
{
    QXmlStreamReader xml;
    QVector<QStringList> tiers, linguistictypes, constraints;

    QFile f(filename); //f("C:\\temp\\diplom\\REC\\rec-template.etf");
    if (!f.open(QFile::ReadOnly)) {
        qDebug() << "Can't open file";
        return;
    }
    xml.setDevice(&f);

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isEndElement()) {
            continue;
        }
        if (!xml.name().length()) {
            continue;
        }
        else {
            if (xml.name() == "CONSTRAINT")
                parseConstraint(xml, constraints);
            else if (xml.name() == "LINGUISTIC_TYPE")
                parseLinguistictype(xml, linguistictypes);
            else if (xml.name() == "TIER")
                parseTier(xml, tiers);
        }

    }

    if (xml.hasError()) {
        qDebug() << xml.errorString();
    }

    emit templateHasParsed(tiers, linguistictypes, constraints);
}

void XmlParser::parseFileHeder(QXmlStreamReader &xml, QString &media)
{
    auto atributes = xml.attributes();
    for (auto a : atributes)
        if (a.name() == "MEDIA_URL")
            media = a.value().toString();
}

void XmlParser::parseTimeOrder(QXmlStreamReader &xml, QStringList &timenames, QVector<int> &timevalues)
{
    while ((xml.name() != "TIME_ORDER") ||!xml.isEndElement()) {
        xml.readNext();
        auto atributes = xml.attributes();
        for (auto a : atributes)
            if (a.name() == "TIME_SLOT_ID")
                timenames.append(a.value().toString());
            else if (a.name() == "TIME_VALUE")
                timevalues.append(a.value().toInt());
    }
}

void XmlParser::parseTierAnnotation(QXmlStreamReader &xml, QString &tierid, QVector<QStringList> &tier, QVector<bool> &isalignable)
{
    auto atributes = xml.attributes();
    for (auto a : atributes)
        if (a.name() == "TIER_ID")
            tierid = a.value().toString();
    QString t1, t2, value, parent, id;
    bool isal = false;
    while ((xml.name() != "TIER") || !xml.isEndElement()) {
        xml.readNext();
        if ((xml.name() == "ANNOTATION") && xml.isEndElement()) {
            if (isal) {
                tier.append(QStringList() << id << value << t1 << t2);
            }
            else {
               tier.append(QStringList() << id << value << parent);

            }
            isalignable.append(isal);

            value = "";
            t1 = "";
            t2 = "";
            parent = "";
            id = "";
            isal = false;
            continue;
        }
        if (xml.name() == "ALIGNABLE_ANNOTATION") {
            isal = true;
            auto atributes = xml.attributes();
            for (auto a : atributes)
                if (a.name() == "TIME_SLOT_REF1") {
                    t1 = a.value().toString();
                }
                else if (a.name() == "ANNOTATION_ID") {
                    id = a.value().toString();
                }
                else if (a.name() == "TIME_SLOT_REF2") {
                    t2 = a.value().toString();
                }
        }
        else if (xml.name() == "REF_ANNOTATION") {
            auto atributes = xml.attributes();
            for (auto a : atributes)
                if (a.name() == "ANNOTATION_ID") {
                    id = a.value().toString();
                }
                else if (a.name() == "ANNOTATION_REF") {
                    parent = a.value().toString();
                }
        }
        else if (xml.name() == "ANNOTATION_VALUE" && xml.isStartElement()) {
            xml.readNext();
            value = xml.text().toString();
        }
    }
}

void XmlParser::parseAnnotation(QString filename)
{
    QXmlStreamReader xml;

    QFile f(filename);
    if (!f.open(QFile::ReadOnly)) {
        qDebug() << "Can't open file";
        return;
    }
    xml.setDevice(&f);

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == "MEDIA_DESCRIPTOR") {
                QString media;
                parseFileHeder(xml, media);
                emit fileHederHasParsed(filename, media);
            }
            else if (xml.name() == "TIME_ORDER") {
                QStringList slotnames;
                QVector<int> slotvalues;
                parseTimeOrder(xml, slotnames, slotvalues);
                emit timeOrderHasParsed(filename, slotnames, slotvalues);
            }
            else if (xml.name() == "TIER") {
                QVector<QStringList> annotations;
                QString tierid;
                QVector<bool> isalignable;
                parseTierAnnotation(xml, tierid, annotations, isalignable);
                emit tierAnnotationHasParsed(filename, tierid, annotations, isalignable);
            }

        }

    }
    if (xml.hasError()) {
        qDebug() << xml.errorString();
    }

}

void XmlParser::parseTier(QXmlStreamReader &xml, QVector<QStringList> &tiers)
{
    QString tierid("NULL"), parentid("NULL"), linguistictypeid("NULL");
    auto atributes = xml.attributes();
    for (auto a : atributes)
        if (a.name() == "TIER_ID")
            tierid = a.value().toString();
        else if (a.name() == "PARENT_REF")
            parentid = a.value().toString();
        else if (a.name() == "LINGUISTIC_TYPE_REF")
            linguistictypeid = a.value().toString();
    QStringList tier;
    tier << tierid << parentid << linguistictypeid;
    tiers << tier;
}

void XmlParser::parseLinguistictype(QXmlStreamReader &xml, QVector<QStringList> &linguistictypes)
{
    QString constraintid("NULL"), linguistictypeid("NULL");
    auto atributes = xml.attributes();
    for (auto a : atributes)
        if (a.name() == "CONSTRAINTS")
            constraintid = a.value().toString();
        else if (a.name() == "LINGUISTIC_TYPE_ID")
            linguistictypeid = a.value().toString();
    QStringList linguistictype;
    linguistictype << constraintid << linguistictypeid;
    linguistictypes << linguistictype;
}

void XmlParser::parseConstraint(QXmlStreamReader &xml, QVector<QStringList> &constraints)
{
    QString constraintid("NULL"), description("");
    auto atributes = xml.attributes();
    for (auto a : atributes)
        if (a.name() == "STEREOTYPE")
            constraintid = a.value().toString();
        else if (a.name() == "DESCRIPTION")
            description = a.value().toString();
    QStringList constraint;
    constraint << constraintid << description;
    constraints << constraint;
}
