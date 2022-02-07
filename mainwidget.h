#pragma once

#include <iostream>
#include <QMainWindow>
#include <QtGui>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include "myderivedmap.h"
#include "ProjectSettings.h"

#define USE_ONLY_STATUS 0

///
/// \brief The MainWidget class
///
class MainWidget: public QMainWindow
{
Q_OBJECT
public:
    MainWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~MainWidget() = default;

Q_SIGNALS:
    void updateZoom(int);

private:
	void populateCombo();

    QString m_applicationName = QString::fromLocal8Bit("FrameBinder");

    myDerivedMap* m_map = nullptr;
    QVBoxLayout* m_vlayout = nullptr;

    QHBoxLayout* m_hMainLayout = nullptr;

    QVBoxLayout* m_vPointsLayout = nullptr;
    QTableWidget* m_frameMapTable = nullptr;
    QLabel* m_frameToMapLabel = nullptr;

    QComboBox* m_combo = nullptr;

    QHBoxLayout* m_hSelectLayout = nullptr;
    QPushButton* m_selectFrame = nullptr;
    QLineEdit* m_selectedFrame = nullptr;

    QHBoxLayout* m_hTransparentLayout = nullptr;
    QLabel* m_transparentLabel = nullptr;
    QSlider* m_transparentSlider = nullptr;

#if !USE_ONLY_STATUS
    QHBoxLayout* m_hlayout = nullptr;
#endif
    QStatusBar* m_statusBar = nullptr;
    QLabel* m_statusZoomLabel = nullptr;
    QLineEdit* m_statusZoomEdit = nullptr;
    QLabel* m_statusLatitudeLabel = nullptr;
    QLineEdit* m_statusLatitudeEdit = nullptr;
    QLabel* m_statusLongitudeLabel = nullptr;
    QLineEdit* m_statusLongitudeEdit = nullptr;

    void ApplyProject(const ProjectSettings& projectSettings);

protected:
	void paintEvent(QPaintEvent*);
    void genStatus(bool fillEdits = true);
private slots:
	void setServer(int);
    void updateZoom();
    void updateGeoCoors();
    void updateEdits();
    void selectFileClick(bool checked);
    void updateTransparent(int transparent);
    void NewProject();
    void OpenProject();
    void SaveProject();
    void NewFrameGeoCoords(const QPolygonF& geoCoords);

    void showEvent(QShowEvent* showEvent);
};
