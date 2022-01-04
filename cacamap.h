/*
Copyright 2010 Jean Fairlie jmfairlie@gmail.com

CacaMap is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/


/** @file cacamap.h
* CacaMap is a Simple Qt OSM Map Widget 
*/

#ifndef CACAMAP_H
#define CACAMAP_H
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QtNetwork>
#include <iostream>
#include <vector>
#include "servermanager.h"

/**
* The quint32 version of QPoint
*/

struct longPoint
{
    quint32 x = 0; /**< x coord. */
    quint32 y = 0; /**< y coord.*/
    longPoint(quint32 x_, quint32 y_) noexcept
        : x(x_), y(y_)
    {
    }
    longPoint() = default;
};

/**
Helper struct that handles coordinate transformations
*/
struct myMercator
{
	static longPoint geoCoordToPixel(QPointF const &,int , int);
	static QPointF pixelToGeoCoord(longPoint const &, int, int);
};

/**
* Struct to define a range of consecutive tiles
* It's used to identify which tiles are visible and need to be rendered/downlaoded
* @see cacaMap::updateTilesToRender()
*/
struct tileSet
{
    int zoom = 0;         /**< zoom level.*/
    qint32 top = 0;       /**< topmost row.*/
    qint32 bottom = 0;    /**< bottommost row.*/
    qint32 left = 0;      /**< leftmostcolumn. */
    qint32 right = 0;     /**< rightmost column. */
    int offsetx = 0;      /**< horizontal offset needed to align the tiles in the wiget.*/
    int offsety = 0;      /**< vertical offset needed to align the tiles in the widget.*/
};

/**
* Used to represent a specific %tile
* @see cacaMap::tileCache
*/
struct tile
{
    int zoom = 0;    /**< zoom level.*/
    qint32 x = 0;    /**< colum number.*/
    qint32 y = 0;    /**< row number.*/
    QString url;     /**< used to identify the %tile when it finishes downloading.*/
};

/**
* maximum space allowed for caching tiles
*/
#define CACHE_MAX 1*1024*1024 //1MB
/**
Main map widget
*/
class cacaMap : public QWidget
{
Q_OBJECT

public:	
	cacaMap(QWidget * _parent=0);
	virtual ~cacaMap();

	void setGeoCoords(QPointF);
	bool zoomIn();
	bool zoomOut();
	bool setZoom(int level);
	QPointF getGeoCoords();
	QStringList getServerNames();
	void setServer(int);
	int getZoom();

private:
    QNetworkAccessManager *manager; /**< manages http requests. */
    tileSet tilesToRender;          /**< range of visible tiles. */
    QHash<QString,int> tileCache;   /**< list of cached tiles (in HDD). */
    QHash<QString,tile> downloadQueue; /**< list of tiles waiting to be downloaded. */
    QHash<QString,int> unavailableTiles; /**< list of tiles that were not found on the server.*/
    bool downloading = false;       /**< flag that indicates if there is a download going on. */
    QString folder;                 /**< root application folder. */
    QMovie loadingAnim;             /**< to show a 'loading' animation for yet unavailable tiles. */
	QPixmap notAvailableTile;
	servermanager servermgr;	

	void renderMap(QPainter &);
	void downloadPicture();
	void loadCache();
	QString getTilePath(int, qint32);
	QPixmap getTilePatch(int,quint32,quint32,int,int,int);

protected:
    int zoom = 16;    /**< Map zoom level. */
    int minZoom = 0;  /**< Minimum zoom level (farthest away).*/
    int maxZoom = 20; /**< Maximum zoom level (closest).*/

    QString cacheDir = "cache";

    int tileSize = 256;                /**< size in px of the square %tile. */
    quint32 cacheSize = 0;             /**< current %tile cache size in bytes. */
    // check QtMobility QGeoCoordinate
    QPointF geocoords = QPointF(-123.140499, 49.313331); /**< current longitude and latitude. */
	QPixmap* imgBuffer;
	QPixmap tmpbuff;
    float buffzoomrate = 1.0;

	bool bufferDirty; /**< image buffer needs to be updated. */	
	void resizeEvent(QResizeEvent*);
	void paintEvent(QPaintEvent *);
	void updateTilesToRender();
	void updateBuffer();
	void updateContent();

protected slots:
	void slotDownloadProgress(qint64, qint64);
	void slotDownloadReady(QNetworkReply *);
	void slotError(QNetworkReply::NetworkError);
};
#endif
