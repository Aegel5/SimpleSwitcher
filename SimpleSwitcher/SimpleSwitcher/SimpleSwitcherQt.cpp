#include "stdafx.h"
#include "SimpleSwitcherQt.h"

#include <QDebug>

void SimpleSwitcherQt::addTask()
{
	qDebug() << "User clicked on the button!";
}

SimpleSwitcherQt::SimpleSwitcherQt(QWidget *parent)
	: QMainWindow(parent)
{

	ui.setupUi(this);

	//connect(ui.pushButton, &QPushButton::clicked, this, &SimpleSwitcherQt::addTask);
}
