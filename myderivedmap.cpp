#include "myderivedmap.h"

///
/// \brief myDerivedMap::myDerivedMap
/// \param parent
///
myDerivedMap::myDerivedMap(QWidget* parent)
    : cacaMap(parent)
{
    std::cout << "derived constructor" << std::endl;
    m_timer = new QTimer(this);

    m_hlayout = new QHBoxLayout;

    m_slider = new MySlider(Qt::Vertical, this);
    m_slider->setTickPosition(QSlider::TicksBothSides);
    m_slider->setRange(minZoom, maxZoom);
    m_slider->setSliderPosition(zoom);
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(updateZoom(int)));
	
    m_hlayout->addWidget(m_slider);
    m_hlayout->addStretch();
    setLayout(m_hlayout);
}

///
/// \brief myDerivedMap::~myDerivedMap
///
myDerivedMap::~myDerivedMap()
{
    delete m_slider;
    delete m_hlayout;
}

///
/// \brief myDerivedMap::AddFrame
/// \param img
/// \param transform
///
bool myDerivedMap::AddFrame(const QString& pathTofile, const QPolygonF& frameGeoPoints)
{
    bool res = m_geoFrame.AddFrame(pathTofile, frameGeoPoints, size(), zoom, tileSize, getGeoCoords());
    update();
    return res;
}

///
/// \brief myDerivedMap::GetFrameGeoPoints
/// \return
///
QPolygonF myDerivedMap::GetFrameGeoPoints() const
{
    return m_geoFrame.GetFrameGeoPoints();
}

///
/// \brief myDerivedMap::SetTransparent
/// \param transparent
///
void myDerivedMap::SetTransparent(int transparent)
{
    m_transparent = transparent;
    update();
}

///
/// \brief myDerivedMap::GetTransparent
/// \return
///
int myDerivedMap::GetTransparent() const
{
   return m_transparent;
}

/**
Saves the screen coordinates of the last click
This is used for scrolling the map
@see myDerived::mouseMoveEvent()
*/
void myDerivedMap::mousePressEvent(QMouseEvent* e)
{
    m_moveMap = !m_geoFrame.IsInFrame(e->pos());
    if (m_moveMap)
        m_mouseAnchor = e->pos();
}

/**
Calculates the length of the mouse drag and
translates it into a new coordinate, map is rerendered
*/
void myDerivedMap::mouseMoveEvent(QMouseEvent* e)
{
    if (m_moveMap)
    {
        QPoint delta = e->pos() - m_mouseAnchor;
        m_mouseAnchor = e->pos();
        longPoint p = myMercator::geoCoordToPixel(getGeoCoords(), zoom, tileSize);

        p.x -= delta.x();
        p.y -= delta.y();
        setGeoCoords(myMercator::pixelToGeoCoord(p, zoom, tileSize), true);

        m_geoFrame.RecalcCoords(zoom, tileSize, getGeoCoords());
    }
    else
    {
        if (m_geoFrame.MouseMove(e->pos(), zoom, tileSize))
            m_geoFrame.RecalcCoords(zoom, tileSize, getGeoCoords());
    }

	updateContent();
	update();
}

///
/// \brief myDerivedMap::mouseDoubleClickEvent
/// \param e
///
void myDerivedMap::mouseDoubleClickEvent(QMouseEvent* e)
{
	//do the zoom-in animation magic
	if (e->button() == Qt::LeftButton)
	{
		QPoint deltapx = e->pos() - QPoint(width()/2,height()/2);
        longPoint currpospx = myMercator::geoCoordToPixel(getGeoCoords(), zoom, tileSize);
		longPoint newpospx;
		newpospx.x = currpospx.x + deltapx.x();
		newpospx.y = currpospx.y + deltapx.y();
        m_destination = myMercator::pixelToGeoCoord(newpospx,zoom,tileSize);
        connect(m_timer,SIGNAL(timeout()),this,SLOT(zoomAnim()));
        m_timer->start(40);
	}
	//do a simple zoom out for now
	else if (e->button() == Qt::RightButton)
	{
		zoomOut();
        m_geoFrame.RecalcCoords(zoom, tileSize, getGeoCoords());
        m_slider->setSliderPosition(zoom);
		update();
	}
}

///
/// \brief myDerivedMap::zoomAnim
///
void myDerivedMap::zoomAnim()
{
	float delta = buffzoomrate - 0.5;
    if (delta > m_minDistance)
	{
        QPointF deltaSpace = m_destination - getGeoCoords();
        setGeoCoords(getGeoCoords() + m_animRate * deltaSpace, true);
        buffzoomrate-= delta*m_animRate;
		updateContent();
	}
	//you are already there
	else
	{
        m_timer->stop();
        disconnect(m_timer,SIGNAL(timeout()),this,SLOT(zoomAnim()));
        setGeoCoords(m_destination, true);
		buffzoomrate = 1.0;
		zoomIn();
        m_geoFrame.RecalcCoords(zoom, tileSize, getGeoCoords());
        m_slider->setSliderPosition(zoom);
	}
	update();
}

///
/// \brief myDerivedMap::updateZoom
/// \param newZoom
///
void myDerivedMap::updateZoom(int newZoom)
{
	setZoom(newZoom);
    m_geoFrame.RecalcCoords(zoom, tileSize, getGeoCoords());
	update();
}

///
/// \brief myDerivedMap::paintEvent
/// \param e
///
void myDerivedMap::paintEvent(QPaintEvent *e)
{
	cacaMap::paintEvent(e);

    if (!m_geoFrame.GetPixmap().isNull())
    {
        QPainter p(this);
        auto currOpacity = p.opacity();
        p.setOpacity(0.01 * m_transparent);
        auto currTransform = p.transform();
        p.setTransform(m_geoFrame.GetTransform());
        p.drawPixmap(0, 0, m_geoFrame.GetPixmap());
        p.setTransform(currTransform);
        p.setOpacity(currOpacity);
    }
}

/**
Widget resize event handler
*/
void myDerivedMap::resizeEvent(QResizeEvent* event)
{
    m_geoFrame.SetNewMapSize(size());
    m_geoFrame.RecalcCoords(zoom, tileSize, getGeoCoords());
    cacaMap::resizeEvent(event);
}
