#include <QtWidgets/QApplication>
#include "testwidget.h"

int main (int argc, char **argv)
{
	QApplication a(argc, argv);
	MainWidget myWidget;
	myWidget.show();
	return a.exec();
}
