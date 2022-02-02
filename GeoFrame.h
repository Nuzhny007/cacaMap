#pragma once

#include "cacamap.h"

///
/// \brief The GeoFrame class
///
class GeoFrame
{
public:
    GeoFrame() = default;

    ///
    /// \brief SetNewMapSize
    /// \param mapSize
    ///
    void SetNewMapSize(QSize mapSize)
    {
        m_mapSize = mapSize;
    }

    ///
    /// \brief AddFrame
    /// \param pathTofile
    /// \param frameGeoPoints
    /// \param mapSize
    /// \param zoom
    /// \param tileSize
    /// \param geoCenter
    /// \return
    ///
    bool AddFrame(const QString& pathTofile, const QPolygonF& frameGeoPoints, QSize mapSize, int zoom, int tileSize, QPointF geoCenter)
    {
        m_mapSize = mapSize;

        m_img.load(pathTofile);
        if (m_img.isNull())
        {
            std::cout << "Can not load frame from " << pathTofile.toStdString() << std::endl;
            return false;
        }

        if (frameGeoPoints.empty())
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
            }
        }
        else
        {
            m_imgGeoCoords = frameGeoPoints;
            RecalcCoords(zoom, tileSize, geoCenter);
        }
#if 0
        std::cout << "'AddFrame: m_imgGeoCoords: ";
        for (int i = 0; i < m_imgGeoCoords.count(); ++i)
        {
            std::cout << "(" << m_imgGeoCoords.at(i).x() << ", " << m_imgGeoCoords.at(i).y() << ") ";
        }
        std::cout << std::endl;
        std::cout << "quadToQuad: " << res << ", matrix: [(" << m_transform.m11() << ", " << m_transform.m12() << ", " << m_transform.m13() << "), "
                  << "(" << m_transform.m21() << ", " << m_transform.m22() << ", " << m_transform.m23() << "), "
                  << "(" << m_transform.m31() << ", " << m_transform.m32() << ", " << m_transform.m33() << ")]" << std::endl;
#endif

        return true;
    }

    ///
    /// \brief RecalcCoords
    /// \param zoom
    /// \param tileSize
    /// \param geoCenter
    ///
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
#if 0
        std::cout << "quadToQuad: " << res << ", matrix: [(" << m_transform.m11() << ", " << m_transform.m12() << ", " << m_transform.m13() << "), "
                  << "(" << m_transform.m21() << ", " << m_transform.m22() << ", " << m_transform.m23() << "), "
                  << "(" << m_transform.m31() << ", " << m_transform.m32() << ", " << m_transform.m33() << ")]" << std::endl;
#endif
    }

    ///
    /// \brief GetdX
    /// \return
    ///
    int GetdX() const
    {
        //return m_transform.m31();
        return m_transform.map(QPoint(0, 0)).x();
    }
    ///
    /// \brief GetdY
    /// \return
    ///
    int GetdY() const
    {
        //return m_transform.m32();
        return m_transform.map(QPoint(0, 0)).y();
    }
    ///
    /// \brief GetPixmap
    /// \return
    ///
    const QPixmap& GetPixmap() const
    {
        return m_img;
    }
    ///
    /// \brief GetTransform
    /// \return
    ///
    const QTransform& GetTransform() const
    {
        return m_transform;
    }
    ///
    /// \brief GetFrameGeoPoints
    /// \return
    ///
    QPolygonF GetFrameGeoPoints() const
    {
       return m_imgGeoCoords;
    }

    ///
    /// \brief IsInFrame
    /// \param pt
    /// \return
    ///
    bool IsInFrame(QPoint pt)
    {
        QRectF imgRect(0, 0, m_img.width(), m_img.height());
        QPolygonF imgPoly(imgRect);
        QPolygonF imgOnMapPoly = m_transform.map(imgPoly);

        m_vertexIndex = -1;
        for (int i = 0; i < imgOnMapPoly.size(); ++i)
        {
            auto vertex = imgOnMapPoly.at(i);
            QRectF vertexRect(vertex.x() - m_vertexRadius, vertex.y() - m_vertexRadius, 2 * m_vertexRadius, 2 * m_vertexRadius);
            if (vertexRect.contains(pt))
            {
                m_vertexIndex = i;
                break;
            }
        }

        bool res = m_vertexIndex >= 0;
        if (!res)
            res = imgOnMapPoly.containsPoint(pt, Qt::OddEvenFill);

        if (res)
            m_mouseAnchor = pt;

#if 0
        std::cout << "IsInFrame: pt = (" << pt.x() << ", " << pt.y() << ") is in Frame: " << res << ", imgOnMapPoly: ";
        for (int i = 0; i < imgOnMapPoly.count(); ++i)
        {
            std::cout << "(" << imgOnMapPoly.at(i).x() << ", " << imgOnMapPoly.at(i).y() << ") ";
        }
        std::cout << std::endl;
#endif

        return res;
    }

    ///
    /// \brief MouseMove
    /// \param pt
    /// \param zoom
    /// \param tileSize
    /// \return
    ///
    bool MouseMove(QPoint pt, int zoom, int tileSize)
    {
        QPoint delta = pt - m_mouseAnchor;
        m_mouseAnchor = pt;

        if (m_vertexIndex >= 0)
        {
                longPoint p = myMercator::geoCoordToPixel(m_imgGeoCoords.at(m_vertexIndex), zoom, tileSize);
                p.x += delta.x();
                p.y += delta.y();
                m_imgGeoCoords[m_vertexIndex] = myMercator::pixelToGeoCoord(p, zoom, tileSize);
        }
        else
        {
            for (QPointF& geoPt : m_imgGeoCoords)
            {
                longPoint p = myMercator::geoCoordToPixel(geoPt, zoom, tileSize);
                p.x += delta.x();
                p.y += delta.y();
                geoPt = myMercator::pixelToGeoCoord(p, zoom, tileSize);
            }
        }

        return true;
    }

private:
    QPixmap m_img;
    QTransform m_transform;

    QSize m_mapSize;
    QPolygonF m_imgGeoCoords;

    QPoint m_mouseAnchor;
    int m_vertexIndex = -1;

    static constexpr int m_vertexRadius = 20;
};
