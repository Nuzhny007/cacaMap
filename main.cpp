#include <QtWidgets/QApplication>
#include "mainwidget.h"

int main (int argc, char **argv)
{
	QApplication a(argc, argv);
	MainWidget myWidget;
	myWidget.show();
	return a.exec();
}
