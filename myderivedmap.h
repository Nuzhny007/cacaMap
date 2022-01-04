#ifndef DERIVEDMAP_H
#define DERIVEDMAP_H

#include <QtWidgets/QLayout>
#include <QtWidgets/QSlider>
#include "cacamap.h"

class myDerivedMap: public cacaMap
{

Q_OBJECT
public:
	myDerivedMap(QWidget* _parent=0);
	~myDerivedMap();
protected:
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseDoubleClickEvent(QMouseEvent*);
private:
	QPoint mouseAnchor;/**< used to keep track of the last mouse click location.*/
	QTimer * timer;
	QHBoxLayout * hlayout;
	
	QSlider * slider;
	QPointF destination; /**< used for dblclick+zoom animations */
    float mindistance = 0.025;/**< used to identify the end of the animation*/
    float animrate = 0.5;
protected slots:
	void zoomAnim();
	void updateZoom(int);
};
#endif


