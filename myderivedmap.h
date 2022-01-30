#pragma once

#include <QtWidgets/QLayout>
#include <QtWidgets/QSlider>
#include "cacamap.h"
#include "GeoFrame.h"

///
/// \brief The MySlider class
///
class MySlider : public QSlider
{
Q_OBJECT
public:
    MySlider(Qt::Orientation orientation, QWidget *parent = nullptr)
        : QSlider(orientation, parent)
    {
    }

protected:
    void paintEvent(QPaintEvent *ev) override
    {
        QSlider::paintEvent(ev);
#if 0
        auto round_value = std::floor(value());
        auto val_text = QString::number(round_value);

        QPainter painter(this);
        painter.setPen(QPen(Qt::black));

        auto font_metrics = QFontMetrics(this->font());
        auto font_width = font_metrics.boundingRect(val_text).width();

        auto rect = geometry();
        int x_pos = rect.width() / 2;
        int y_pos = rect.height() / 2;

        if (this->orientation() == Qt::Horizontal)
        {
            x_pos = rect.width() - font_width - 5;
            y_pos = rect.height() * 0.75;
        }
        else if (this->orientation() == Qt::Vertical)
        {
            x_pos = rect.width() - font_width;
            y_pos = rect.height() - 5;
        }
        else
        {
            return;
        }
        painter.drawText(QPoint(x_pos, y_pos), val_text);
#endif
    }
};

///
/// \brief The myDerivedMap class
///
class myDerivedMap: public cacaMap
{
Q_OBJECT
public:
	myDerivedMap(QWidget* _parent=0);
    ~myDerivedMap();

    bool AddFrame(const QString& pathTofile, QTransform transform);
    QTransform GetTransform() const;
    void SetTransparent(int transparent);
    int GetTransparent() const;

protected:
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseDoubleClickEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);

private:
    QPoint m_mouseAnchor; /**< used to keep track of the last mouse click location.*/
    QTimer* m_timer;
    QHBoxLayout* m_hlayout;
	
    MySlider* m_slider;
    QPointF m_destination; /**< used for dblclick+zoom animations */
    float m_minDistance = 0.025; /**< used to identify the end of the animation*/
    float m_animRate = 0.5;

    GeoFrame m_geoFrame;
    int m_transparent = 50;
protected slots:
	void zoomAnim();
    void updateZoom(int);
};
