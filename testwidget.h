#pragma once

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

#include <QtGui>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include "myderivedmap.h"
#include <iostream>

class testWidget: public QWidget
{
Q_OBJECT
public:
	testWidget(QWidget* parent=0);
	~testWidget();
private:
	void populateCombo();

    myDerivedMap* m_map;
    QVBoxLayout* m_vlayout;

    QComboBox* m_combo;
    QHBoxLayout* m_hlayout;
    QStatusBar* m_statusBar;
    QLabel* m_statusZoomLabel;
    QLineEdit* m_statusZoomEdit;
    QLabel* m_statusLatitudeLabel;
    QLineEdit* m_statusLatitudeEdit;
    QLabel* m_statusLongitudeLabel;
    QLineEdit* m_statusLongitudeEdit;

protected:
	void paintEvent(QPaintEvent*);
    void genStatus(bool fillEdits = true);
private slots:
	void setServer(int);
    void updateZoom();
    void updateGeoCoors();
    void updateEdits();

    void showEvent(QShowEvent* showEvent);
};
