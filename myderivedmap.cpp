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

/**
Saves the screen coordinates of the last click
This is used for scrolling the map
@see myDerived::mouseMoveEvent()
*/
void myDerivedMap::mousePressEvent(QMouseEvent* e)
{
    m_mouseAnchor = e->pos();
}

/**
Calculates the length of the mouse drag and
translates it into a new coordinate, map is rerendered
*/
void myDerivedMap::mouseMoveEvent(QMouseEvent* e)
{
    QPoint delta = e->pos() - m_mouseAnchor;
    m_mouseAnchor = e->pos();
    longPoint p = myMercator::geoCoordToPixel(getGeoCoords(), zoom, tileSize);
	
	p.x-= delta.x();
	p.y-= delta.y();
    setGeoCoords(myMercator::pixelToGeoCoord(p, zoom, tileSize), true);
	updateContent();
	update();
}

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
        m_slider->setSliderPosition(zoom);
		update();
	}
}

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
        m_slider->setSliderPosition(zoom);
	}
	update();
}

void myDerivedMap::updateZoom(int newZoom)
{
	setZoom(newZoom);
	update();
}

void myDerivedMap::paintEvent(QPaintEvent *e)
{
	cacaMap::paintEvent(e);
}
