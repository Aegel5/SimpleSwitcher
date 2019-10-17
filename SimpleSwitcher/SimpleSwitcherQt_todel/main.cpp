#include "stdafx.h"
#include "SimpleSwitcherQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SimpleSwitcherQt w;
	w.show();
	return a.exec();
}
