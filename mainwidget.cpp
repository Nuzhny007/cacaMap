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
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include "mainwidget.h"

///
/// \brief MainWidget::testWidget
/// \param parent
///
MainWidget::MainWidget(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    setWindowTitle(m_applicationName);

    Qt::WindowFlags wflags = windowFlags();
    wflags |= Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint;
    wflags &= ~Qt::WindowContextHelpButtonHint | Qt::MSWindowsFixedSizeDialogHint;
    setWindowFlags(wflags);

    const QScreen* screen = QGuiApplication::primaryScreen();
    QRect gRect = QRect(0, 0, 3 * screen->size().width() / 4, 3 * screen->size().height() / 4);
    gRect.moveCenter(screen->geometry().center());
    setGeometry(gRect);
    setMinimumSize(1280, 720);

    m_hMainLayout = new QHBoxLayout();
    QWidget *w = new QWidget();
    w->setLayout(m_hMainLayout);
    setCentralWidget(w);

    m_vlayout = new QVBoxLayout(this);

    QMenu* mnu = menuBar()->addMenu(QString::fromLocal8Bit("File"));
    QAction* newAct = mnu->addAction(QString::fromLocal8Bit("New project"), this, SLOT(NewProject()));
    newAct->setShortcuts(QKeySequence::New);
    QAction* openAct = mnu->addAction(QString::fromLocal8Bit("Open project"), this, SLOT(OpenProject()));
    openAct->setShortcuts(QKeySequence::Open);
    QAction* saveAsAct = mnu->addAction(QString::fromLocal8Bit("Save project"), this, SLOT(SaveProject()));
    saveAsAct->setShortcuts(QKeySequence::Save);
    mnu->addSeparator();
    QAction* exitAct = mnu->addAction(QString::fromLocal8Bit("Exit"), this, SLOT(close()));
    exitAct->setShortcuts(QKeySequence::Quit);

    m_map = new myDerivedMap(this);

    m_combo = new QComboBox(this);
    populateCombo();
    m_combo->setCurrentIndex(m_map->getServer());
    m_vlayout->addWidget(m_combo);
    connect(m_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(setServer(int)));

    m_hSelectLayout = new QHBoxLayout(this);
    m_selectedFrame = new QLineEdit(this);
    m_selectedFrame->setPlaceholderText("Sceenshot or video file from camera");
    m_selectedFrame->setReadOnly(true);
    m_selectFrame = new QPushButton(this);
    m_selectFrame->setText("...");
    m_selectFrame->setToolTip("Select sceenshot or video file from camera");
    connect(m_selectFrame, SIGNAL(clicked(bool)), this, SLOT(selectFileClick(bool)));
    m_hSelectLayout->addWidget(m_selectedFrame);
    m_hSelectLayout->addWidget(m_selectFrame);
    m_vlayout->addLayout(m_hSelectLayout);

    m_hTransparentLayout = new QHBoxLayout(this);
    m_transparentLabel = new QLabel(this);
    m_transparentLabel->setText("Transparent: " + QString::number(m_map->GetTransparent()));
    m_transparentSlider = new QSlider(Qt::Horizontal, this);
    m_transparentSlider->setTickPosition(QSlider::TicksBothSides);
    m_transparentSlider->setRange(1, 100);
    m_transparentSlider->setSliderPosition(m_map->GetTransparent());
    connect(m_transparentSlider, SIGNAL(valueChanged(int)), this, SLOT(updateTransparent(int)));
    m_hTransparentLayout->addWidget(m_transparentLabel);
    m_hTransparentLayout->addWidget(m_transparentSlider);
    m_vlayout->addLayout(m_hTransparentLayout);

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

#if USE_ONLY_STATUS
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

    m_vPointsLayout = new QVBoxLayout(this);
    m_frameToMapLabel = new QLabel(this);
    m_frameToMapLabel->setText("Frame to Map bindings");
    m_vPointsLayout->addWidget(m_frameToMapLabel);
    m_frameMapTable = new QTableWidget(this);
    m_frameMapTable->setColumnCount(2);
    m_frameMapTable->setShowGrid(true);
    m_frameMapTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_frameMapTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_frameMapTable->setHorizontalHeaderLabels(QStringList() << tr("(x; y)") << tr("(lat; lon)"));
    //m_frameMapTable->horizontalHeader()->setStretchLastSection(true);
    //m_frameMapTable->hideColumn(0);
    m_vPointsLayout->addWidget(m_frameMapTable);

    //QSplitter* splitter = new QSplitter(this);
    //splitter->addl
    m_hMainLayout->addLayout(m_vlayout);
    m_hMainLayout->addLayout(m_vPointsLayout);
    m_hMainLayout->setStretch(0, 80);
    m_hMainLayout->setStretch(1, 20);

    connect(m_statusZoomEdit, SIGNAL(editingFinished()), this, SLOT(updateZoom()));
    connect(m_statusLatitudeEdit, SIGNAL(editingFinished()), this, SLOT(updateGeoCoors()));
    connect(m_statusLongitudeEdit, SIGNAL(editingFinished()), this, SLOT(updateGeoCoors()));
    connect(m_map, SIGNAL(updateParams()), this, SLOT(updateEdits()));
    connect(this, SIGNAL(updateZoom(int)), m_map, SLOT(updateZoom(int)));
    connect(m_map, SIGNAL(NewFrameGeoCoords(const FrameBinding&)), this, SLOT(NewFrameGeoCoords(const FrameBinding&)));
    connect(this, SIGNAL(CtrlKey(bool)), m_map, SLOT(CtrlKey(bool)));
    connect(this, SIGNAL(AltKey(bool)), m_map, SLOT(AltKey(bool)));

    //setLayout(m_vlayout);
}

///
/// \brief MainWidget::showEvent
/// \param showEvent
///
void MainWidget::showEvent(QShowEvent* showEvent)
{
    QWidget::showEvent(showEvent);
    activateWindow();
}

///
/// \brief MainWidget::ApplyProject
/// \param projectSettings
///
void MainWidget::ApplyProject(const ProjectSettings& projectSettings)
{
    m_map->setZoom(projectSettings.m_zoom);
    m_map->setGeoCoords(QPointF(projectSettings.m_longitude, projectSettings.m_latitude), true);
    genStatus(true);

    bool res = m_map->AddFrame(projectSettings.m_frameFileName, projectSettings.m_frameBinding);
    m_selectedFrame->setText(projectSettings.m_frameFileName);
    QString statusMessage = QString::asprintf("File %s is opened %s", projectSettings.m_frameFileName.toStdString().c_str(), res ? "" : "with error");
    m_statusBar->showMessage(statusMessage, 10000);
}

///
/// \b  rief MainWidget::NewProject
///
void MainWidget::NewProject()
{
    ProjectSettings projectSettings;
    ApplyProject(projectSettings);
}

///
/// \brief MainWidget::OpenProject
///
void MainWidget::OpenProject()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open project"), "...", tr("Xml Files (*.xml)"));
    emit CtrlKey(false);
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, m_applicationName, tr("Cannot read file %1:\n%2.").arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    ProjectSettings projectSettings;
    if (projectSettings.Read(&file))
    {
        QMessageBox::warning(this, m_applicationName, tr("Parse error in file %1:\n\n%2").arg(QDir::toNativeSeparators(fileName), projectSettings.ReadError()));
    }
    else
    {
        statusBar()->showMessage(tr("Project loaded from ") + fileName, 10000);
        ApplyProject(projectSettings);
    }
}

///
/// \brief MainWidget::SaveProject
///
void MainWidget::SaveProject()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save current project"), "..."/*QDir::currentPath()*/, tr("Xml Files (*.xml)"));
    emit CtrlKey(false);
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, m_applicationName, tr("Cannot write file %1:\n%2.").arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }
    ProjectSettings projectSettings;
    projectSettings.m_zoom = m_map->getZoom();
    auto geoCoords = m_map->getGeoCoords();
    projectSettings.m_latitude = geoCoords.y();
    projectSettings.m_longitude = geoCoords.x();
    projectSettings.m_frameFileName = m_selectedFrame->text();
    projectSettings.m_frameBinding = m_map->GetFrameGeoPoints();
    projectSettings.Write(&file);
}

///
/// \brief MainWidget::GenStatus
/// \return
///
void MainWidget::genStatus(bool fillEdits)
{
    QPointF geoCoords = m_map->getGeoCoords();
    if (fillEdits)
    {
        m_statusZoomEdit->setText(QString::number(m_map->getZoom()));
        m_statusLatitudeEdit->setText(QString::number(geoCoords.y(), 'g', 8));
        m_statusLongitudeEdit->setText(QString::number(geoCoords.x(), 'g', 9));
    }
#if !USE_ONLY_STATUS
    QString res = QString::asprintf("zoom %d, latitude %.6f, longitude %.6f", m_map->getZoom(), geoCoords.y(), geoCoords.x());
    m_statusBar->showMessage(res, 10000);
#endif
}

///
/// \brief MainWidget::updateEdits
///
void MainWidget::updateEdits()
{
    genStatus(true);
}

///
/// \brief MainWidget::selectFileClick
/// \param checked
///
void MainWidget::selectFileClick(bool /*checked*/)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open image or video"), "...", tr("Files (*.png *.jpg *.bmp, *avi, *.mp4)"));
    bool res = m_map->AddFrame(fileName, FrameBinding());
    if (res)
        m_selectedFrame->setText(fileName);
    QString statusMessage = QString::asprintf("File %s is opened %s", fileName.toStdString().c_str(), res ? "" : "with error");
    m_statusBar->showMessage(statusMessage, 10000);
    emit CtrlKey(false);
}

///
/// \brief MainWidget::populateCombo
///
void MainWidget::populateCombo()
{
    QStringList l = m_map->getServerNames();

    for (int i = 0; i < l.size(); ++i)
    {
        m_combo->addItem(l.at(i), QVariant(i));
    }
}

///
/// \brief MainWidget::paintEvent
///
void MainWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
}

///
/// \brief MainWidget::setServer
/// \param index
///
void MainWidget::setServer(int index)
{
    m_map->setServer(index);
    genStatus(true);
}

///
/// \brief MainWidget::updateZoom
///
void MainWidget::updateZoom()
{
    int zoom = m_statusZoomEdit->text().toInt();
    emit updateZoom(zoom);
}

///
/// \brief MainWidget::updateGeoCoors
///
void MainWidget::updateGeoCoors()
{
    QPointF geoCoords;
    geoCoords.setY(m_statusLatitudeEdit->text().toDouble());
    geoCoords.setX(m_statusLongitudeEdit->text().toDouble());
    m_map->setGeoCoords(geoCoords, true);
}

///
/// \brief MainWidget::updateTransparent
/// \param transparent
///
void MainWidget::updateTransparent(int transparent)
{
    m_transparentLabel->setText("Transparent: " + QString::number(transparent));
    m_map->SetTransparent(transparent);
}

///
/// \brief MainWidget::NewFrameGeoCoords
/// \param geoCoords
///
void MainWidget::NewFrameGeoCoords(const FrameBinding& geoCoords)
{
    for (int i = 0; i < geoCoords.m_geoPoints.size(); ++i)
    {
        if (m_frameMapTable->rowCount() < i + 1)
            m_frameMapTable->insertRow(i);
        m_frameMapTable->setItem(i, 0, new QTableWidgetItem(QString("(%1; %2)").arg(geoCoords.m_framePoints[i].x()).arg(geoCoords.m_framePoints[i].y())));
        m_frameMapTable->setItem(i, 1, new QTableWidgetItem(QString("(%1; %2)").arg(geoCoords.m_geoPoints[i].y()).arg(geoCoords.m_geoPoints[i].x())));
    }
    for (int i = geoCoords.m_geoPoints.size(); i < m_frameMapTable->rowCount(); ++i)
    {
        m_frameMapTable->removeRow(i);
    }

    m_frameMapTable->resizeColumnsToContents();
}

///
/// \brief MainWidget::keyPressEvent
/// \param event
///
void MainWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
        emit CtrlKey(true);
    else if (event->key() == Qt::Key_Alt)
        emit AltKey(true);
}

///
/// \brief MainWidget::keyReleaseEvent
/// \param event
///
void MainWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
        emit CtrlKey(false);
    else if (event->key() == Qt::Key_Alt)
        emit AltKey(false);
}
