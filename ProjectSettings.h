#pragma once

#include <iostream>

///
/// \brief The ProjectSettings class
///
class ProjectSettings
{
public:
    double m_latitude = 60.0;
    double m_longitude = 30.0;
    int m_zoom = 18;
    QString m_frameFileName;
    QTransform m_frameTransform;

    QXmlStreamReader m_readXml;

    static inline QString versionAttribute()
    {
        return QStringLiteral("version");
    }

    bool Read(QIODevice* device)
    {
        m_readXml.setDevice(device);
        std::cout << "Set devide: " << m_readXml.name().toString().toStdString() << std::endl;

        if (m_readXml.readNextStartElement())
        {
            std::cout << "Read first: " << m_readXml.name().toString().toStdString() << std::endl;
            if (m_readXml.name() == QLatin1String("project")
                    && m_readXml.attributes().value(versionAttribute()) == QLatin1String("1.0"))
            {
                while (m_readXml.readNextStartElement())
                {
                    std::cout << "Read next: " << m_readXml.name().toString().toStdString() << ", attributes " << m_readXml.attributes().size() << " has val: " << m_readXml.attributes().hasAttribute("val") << std::endl;

                    QString readedName = "None";
                    if (m_readXml.name() == QLatin1String("zoom"))
                    {
                        m_zoom = m_readXml.attributes().value(QStringLiteral("val")).toInt();
                        readedName = "Zoom: " + QString::number(m_zoom);
                    }
                    else if (m_readXml.name() == QLatin1String("latitude"))
                    {
                        m_latitude = m_readXml.attributes().value(QStringLiteral("val")).toDouble();
                        readedName = "Latitude: " + QString::number(m_latitude);
                    }
                    else if (m_readXml.name() == QLatin1String("longitude"))
                    {
                        m_longitude = m_readXml.attributes().value(QStringLiteral("val")).toDouble();
                        readedName = "Longitude: " + QString::number(m_longitude);
                    }
                    else if (m_readXml.name() == QLatin1String("file_name"))
                    {
                        m_frameFileName = m_readXml.attributes().value(QStringLiteral("val")).toString();
                        readedName = "File name: " + m_frameFileName;
                    }
                    m_readXml.skipCurrentElement();

                    std::cout << "Readed " << readedName.toStdString() << std::endl;
                }
                m_readXml.skipCurrentElement();
            }
            else
            {
                m_readXml.raiseError(QObject::tr("The file is not an xml version 1.0 file."));
            }
        }

        return !m_readXml.error();
    }

    QString ReadError() const
    {
        return QObject::tr("%1\nLine %2, column %3")
                .arg(m_readXml.errorString())
                .arg(m_readXml.lineNumber())
                .arg(m_readXml.columnNumber());
    }

    bool Write(QIODevice* device)
    {
        QXmlStreamWriter writeXml;
        writeXml.setDevice(device);
        writeXml.setAutoFormatting(true);

        writeXml.writeStartDocument();
        //m_writeXml.writeDTD(QStringLiteral("<!DOCTYPE xbel>"));
        writeXml.writeStartElement(QStringLiteral("project"));
        writeXml.writeAttribute(versionAttribute(), QStringLiteral("1.0"));

        writeXml.writeStartElement("zoom");
        writeXml.writeAttribute("val", QString::number(m_zoom));
        writeXml.writeEndElement();

        writeXml.writeStartElement("latitude");
        writeXml.writeAttribute("val", QString::number(m_latitude, 'g', 9));
        writeXml.writeEndElement();

        writeXml.writeStartElement("longitude");
        writeXml.writeAttribute("val", QString::number(m_longitude, 'g', 10));
        writeXml.writeEndElement();

        writeXml.writeStartElement("file_name");
        writeXml.writeAttribute("val", m_frameFileName);
        writeXml.writeEndElement();

        writeXml.writeEndDocument();
        return true;
    }
};
