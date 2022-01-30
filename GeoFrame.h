#pragma once

#include "cacamap.h"

///
/// \brief The GeoFrame class
///
class GeoFrame
{
public:
    GeoFrame() = default;

    void SetNewMapSize(QSize mapSize)
    {
        m_mapSize = mapSize;
    }

    bool AddFrame(const QString& pathTofile, QSize mapSize, int zoom, int tileSize, QPointF geoCenter)
    {
        m_mapSize = mapSize;
        m_mapGeoCoords = QPolygonF({ myMercator::pixelToGeoCoord(longPoint(0, 0), zoom, tileSize),
                                     myMercator::pixelToGeoCoord(longPoint(mapSize.width(), 0), zoom, tileSize),
                                     myMercator::pixelToGeoCoord(longPoint(mapSize.width(), mapSize.height()), zoom, tileSize),
                                     myMercator::pixelToGeoCoord(longPoint(0, mapSize.height()), zoom, tileSize)});

        m_transformedImg = QPixmap();
        m_img.load(pathTofile);

        if (!m_img.isNull())
        {
            QRectF fromRect(0, 0, m_img.width(), m_img.height());
            QPolygonF fromPoly(fromRect);
            fromPoly.pop_back();
            QRectF toRect(mapSize.width() / 4., mapSize.height() / 4., mapSize.width() / 2., mapSize.height() / 2.);
            QPolygonF toPoly(toRect);
            toPoly.pop_back();
            bool res = QTransform::quadToQuad(fromPoly, toPoly, m_transform);
            if (res)
            {
                m_transformedImg = m_img.transformed(m_transform);

                longPoint pixCenter(mapSize.width() / 2, mapSize.height() / 2);
                longPoint pix = myMercator::geoCoordToPixel(geoCenter, zoom, tileSize);
                auto Pix2Lp = [pixCenter, pix](int x, int y)
                {
                    return longPoint(pix.x - pixCenter.x + x, pix.y - pixCenter.y + y);
                };

                m_imgGeoCoords = QPolygonF({ myMercator::pixelToGeoCoord(Pix2Lp(toRect.x(), toRect.y()), zoom, tileSize),
                                             myMercator::pixelToGeoCoord(Pix2Lp(toRect.right(), toRect.y()), zoom, tileSize),
                                             myMercator::pixelToGeoCoord(Pix2Lp(toRect.right(), toRect.bottom()), zoom, tileSize),
                                             myMercator::pixelToGeoCoord(Pix2Lp(toRect.x(), toRect.bottom()), zoom, tileSize)});
#if 0
                std::cout << "'AddFrame: m_imgGeoCoords: ";
                for (int i = 0; i < m_imgGeoCoords.count(); ++i)
                {
                    std::cout << "(" << m_imgGeoCoords.at(i).x() << ", " << m_imgGeoCoords.at(i).y() << ") ";
                }
                std::cout << std::endl;
#endif
            }
            else
                m_transformedImg = m_img;
#if 0
            std::cout << "quadToQuad: " << res << ", matrix: [(" << m_transform.m11() << ", " << m_transform.m12() << ", " << m_transform.m13() << "), "
                      << "(" << m_transform.m21() << ", " << m_transform.m22() << ", " << m_transform.m23() << "), "
                      << "(" << m_transform.m31() << ", " << m_transform.m32() << ", " << m_transform.m33() << ")]" << std::endl;
#endif
        }
        return !m_img.isNull();
    }

    void RecalcCoords(int zoom, int tileSize, QPointF geoCenter)
    {
        if (m_img.isNull())
            return;

        QRectF fromRect(0, 0, m_img.width(), m_img.height());
        QPolygonF fromPoly(fromRect);
        fromPoly.pop_back();

        longPoint pixCenter(m_mapSize.width() / 2, m_mapSize.height() / 2);
        longPoint pix = myMercator::geoCoordToPixel(geoCenter, zoom, tileSize);

        auto p1 = myMercator::geoCoordToPixel(m_imgGeoCoords.at(0), zoom, tileSize);
        auto p2 = myMercator::geoCoordToPixel(m_imgGeoCoords.at(1), zoom, tileSize);
        auto p3 = myMercator::geoCoordToPixel(m_imgGeoCoords.at(2), zoom, tileSize);
        auto p4 = myMercator::geoCoordToPixel(m_imgGeoCoords.at(3), zoom, tileSize);

        auto Lp2Pix = [pixCenter, pix](quint32 x, quint32 y)
        {
            QPointF pt(x - pix.x + pixCenter.x, y - pix.y + pixCenter.y);
            //std::cout << "Lp2Pix: x = " << x << " -> " << pt.x() << ", y = " << y << " -> " << pt.y() << std::endl;
            return pt;
        };

        QPolygonF toPoly({ Lp2Pix(p1.x, p1.y), Lp2Pix(p2.x, p2.y), Lp2Pix(p3.x, p3.y), Lp2Pix(p4.x, p4.y) });

#if 0
        std::cout << "pixCenter(" << pixCenter.x << ", " << pixCenter.y << "), geoCenter(" << geoCenter.x() << ", " << geoCenter.y() << "), pix(" << pix.x << ", " << pix.y << ")" << std::endl;
        std::cout << "'RecalcCoords: toPoly: ";
        for (int i = 0; i < toPoly.count(); ++i)
        {
            std::cout << "(" << toPoly.at(i).x() << ", " << toPoly.at(i).y() << ") ";
        }
        std::cout << std::endl;
#endif

        bool res = QTransform::quadToQuad(fromPoly, toPoly, m_transform);
        if (res)
            m_transformedImg = m_img.transformed(m_transform);
        else
            m_transformedImg = m_img;
#if 0
        std::cout << "quadToQuad: " << res << ", matrix: [(" << m_transform.m11() << ", " << m_transform.m12() << ", " << m_transform.m13() << "), "
                  << "(" << m_transform.m21() << ", " << m_transform.m22() << ", " << m_transform.m23() << "), "
                  << "(" << m_transform.m31() << ", " << m_transform.m32() << ", " << m_transform.m33() << ")]" << std::endl;
#endif
    }

    int GetdX() const
    {
        return m_transform.m31();
    }

    int GetdY() const
    {
        return m_transform.m32();
    }

    const QPixmap& GetPixmap() const
    {
        return m_transformedImg;
    }

private:
    QPixmap m_img;
    QPixmap m_transformedImg;
    QTransform m_transform;

    QSize m_mapSize;
    QPolygonF m_mapGeoCoords;
    QPolygonF m_imgGeoCoords;
};
