#pragma once

#include <iostream>
#include <QString>
#include <QPolygonF>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "GeoFrame.h"

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
    FrameBinding m_frameBinding;

    QXmlStreamReader m_readXml;

    ///
    /// \brief versionAttribute
    /// \return
    ///
    static inline QString versionAttribute()
    {
        return QStringLiteral("version");
    }

    ///
    /// \brief Read
    /// \param device
    /// \return
    ///
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
                    else if (m_readXml.name() == QLatin1String("geo_point"))
                    {
                        QPointF geoPt;
                        geoPt.setY(m_readXml.attributes().value(QStringLiteral("lat")).toDouble());
                        geoPt.setX(m_readXml.attributes().value(QStringLiteral("lon")).toDouble());
                        m_frameBinding.m_geoPoints.push_back(geoPt);
                        readedName = "Geo point[" + QString::number(m_frameBinding.m_geoPoints.size() - 1) + "]: lat = " + QString::number(geoPt.x()) + ", lon = " + QString::number(geoPt.y());
                    }
                    else if (m_readXml.name() == QLatin1String("frame_point"))
                    {
                        QPointF framePt;
                        framePt.setX(m_readXml.attributes().value(QStringLiteral("x")).toDouble());
                        framePt.setY(m_readXml.attributes().value(QStringLiteral("y")).toDouble());
                        m_frameBinding.m_framePoints.push_back(framePt);
                        readedName = "Frame point[" + QString::number(m_frameBinding.m_framePoints.size() - 1) + "]: x = " + QString::number(framePt.x()) + ", y = " + QString::number(framePt.y());
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

    ///
    /// \brief ReadError
    /// \return
    ///
    QString ReadError() const
    {
        return QObject::tr("%1\nLine %2, column %3")
                .arg(m_readXml.errorString())
                .arg(m_readXml.lineNumber())
                .arg(m_readXml.columnNumber());
    }

    ///
    /// \brief Write
    /// \param device
    /// \return
    ///
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

        for (auto geoPt : qAsConst(m_frameBinding.m_geoPoints))
        {
            writeXml.writeStartElement("geo_point");
            writeXml.writeAttribute("lat", QString::number(geoPt.y(), 'g', 9));
            writeXml.writeAttribute("lon", QString::number(geoPt.x(), 'g', 10));
            writeXml.writeEndElement();
        }
        for (auto framePt : qAsConst(m_frameBinding.m_framePoints))
        {
            writeXml.writeStartElement("frame_point");
            writeXml.writeAttribute("x", QString::number(framePt.x(), 'g', 10));
            writeXml.writeAttribute("y", QString::number(framePt.y(), 'g', 10));
            writeXml.writeEndElement();
        }

        writeXml.writeEndDocument();
        return true;
    }
};
