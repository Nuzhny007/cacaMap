/*
Copyright 2010 Jean Fairlie jmfairlie@gmail.com

This program  is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#include "testwidget.h"

///
/// \brief testWidget::testWidget
/// \param parent
///
testWidget::testWidget(QWidget* parent)
    : QWidget(parent)
{
    m_map = new myDerivedMap(this);

    m_vlayout = new QVBoxLayout(this);
    m_combo = new QComboBox(this);
    populateCombo();
    m_vlayout->addWidget(m_combo);

    connect(m_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(setServer(int)));

    m_vlayout->addWidget(m_map);

    m_statusBar = new QStatusBar(this);
    m_statusZoomLabel = new QLabel(this);
    m_statusZoomLabel->setText("Zoom:");
    m_statusZoomEdit = new QLineEdit(this);
    m_statusZoomEdit->setValidator(new QIntValidator(m_map->getMinZoom(), m_map->getMaxZoom(), this));
    m_statusZoomEdit->setReadOnly(false);

    m_statusLatitudeLabel = new QLabel(this);
    m_statusLatitudeLabel->setText("Latitude:");
    m_statusLatitudeEdit = new QLineEdit(this);
    //m_statusLatitudeEdit->setValidator(new QDoubleValidator(0, 90, 8, this));

    m_statusLongitudeLabel = new QLabel(this);
    m_statusLongitudeLabel->setText("Longitude:");
    m_statusLongitudeEdit = new QLineEdit(this);
    //m_statusLongitudeEdit->setValidator(new QDoubleValidator(-180, 180, 9, this));

#if 0
    m_statusBar->addPermanentWidget(m_statusZoomLabel);
    m_statusBar->addPermanentWidget(m_statusZoomEdit);
    m_statusBar->addPermanentWidget(m_statusLatitudeLabel);
    m_statusBar->addPermanentWidget(m_statusLatitudeEdit);
    m_statusBar->addPermanentWidget(m_statusLongitudeLabel);
    m_statusBar->addPermanentWidget(m_statusLongitudeEdit);
#else
    m_hlayout = new QHBoxLayout(this);
    m_hlayout->addWidget(m_statusZoomLabel);
    m_hlayout->addWidget(m_statusZoomEdit);
    m_hlayout->addWidget(m_statusLatitudeLabel);
    m_hlayout->addWidget(m_statusLatitudeEdit);
    m_hlayout->addWidget(m_statusLongitudeLabel);
    m_hlayout->addWidget(m_statusLongitudeEdit);
    m_vlayout->addLayout(m_hlayout);
#endif

    genStatus(true);
    m_vlayout->addWidget(m_statusBar, 0);

    connect(m_statusZoomEdit, SIGNAL(editingFinished()), this, SLOT(updateZoom()));
    connect(m_statusLatitudeEdit, SIGNAL(editingFinished()), this, SLOT(updateGeoCoors()));
    connect(m_statusLongitudeEdit, SIGNAL(editingFinished()), this, SLOT(updateGeoCoors()));
    connect(m_map, SIGNAL(updateParams()), this, SLOT(updateEdits()));

    setLayout(m_vlayout);

    QSize size(1280, 720);
    resize(size);

    setWindowModality(Qt::WindowModal);
    m_statusZoomEdit->activateWindow();
}

///
/// \brief testWidget::showEvent
/// \param showEvent
///
void testWidget::showEvent(QShowEvent* showEvent)
{
    QWidget::showEvent(showEvent);
    activateWindow();
}

///
/// \brief testWidget::GenStatus
/// \return
///
void testWidget::genStatus(bool fillEdits)
{
    QPointF geoCoords = m_map->getGeoCoords();
    if (fillEdits)
    {
        m_statusZoomEdit->setText(QString::number(m_map->getZoom()));
        m_statusLatitudeEdit->setText(QString::number(geoCoords.y(), 'g', 8));
        m_statusLongitudeEdit->setText(QString::number(geoCoords.x(), 'g', 9));
    }
    QString res = QString::asprintf("zoom %d, latitude %.6f, longitude %.6f", m_map->getZoom(), geoCoords.y(), geoCoords.x());
    m_statusBar->showMessage(res);
}

///
/// \brief testWidget::updateEdits
///
void testWidget::updateEdits()
{
    QPointF geoCoords = m_map->getGeoCoords();
    m_statusZoomEdit->setText(QString::number(m_map->getZoom()));
    m_statusLatitudeEdit->setText(QString::number(geoCoords.y(), 'g', 8));
    m_statusLongitudeEdit->setText(QString::number(geoCoords.x(), 'g', 9));
}

///
/// \brief testWidget::populateCombo
///
void testWidget::populateCombo()
{
    QStringList l = m_map->getServerNames();

    for (int i = 0; i < l.size(); ++i)
    {
        m_combo->addItem(l.at(i), QVariant(i));
    }
}

///
/// \brief testWidget::paintEvent
///
void testWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    genStatus(false);
}

///
/// \brief testWidget::~testWidget
///
testWidget::~testWidget()
{
    delete m_map;
    delete m_vlayout;
}

///
/// \brief testWidget::setServer
/// \param index
///
void testWidget::setServer(int index)
{
    m_map->setServer(index);
    genStatus(true);
}

///
/// \brief testWidget::updateZoom
///
void testWidget::updateZoom()
{
    int zoom = m_statusZoomEdit->text().toInt();
    m_map->setZoom(zoom);
}

///
/// \brief testWidget::updateGeoCoors
///
void testWidget::updateGeoCoors()
{
    QPointF geoCoords;
    geoCoords.setY(m_statusLatitudeEdit->text().toDouble());
    geoCoords.setX(m_statusLongitudeEdit->text().toDouble());
    std::cout << "updateGeoCoors: " << geoCoords.y() << ", " << geoCoords.x() << std::endl;
    m_map->setGeoCoords(geoCoords, true);
}
