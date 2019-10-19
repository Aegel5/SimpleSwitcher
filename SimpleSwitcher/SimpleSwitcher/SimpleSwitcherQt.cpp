#include "stdafx.h"
#include "SimpleSwitcherQt.h"


#include <QDebug>


//void SimpleSwitcherQt::ChangeHotKey(QLineEdit* lineEdit)
//{
//	qDebug() << "User clicked on the button!";
//}

SimpleSwitcherQt::SimpleSwitcherQt(QWidget *parent)
	: QMainWindow(parent)
{

	ui.setupUi(this);
	instance = this;


	//connect(ui.lineEdit_lastword, &QLineEdit::mouseDoubleClickEvent, this, &SimpleSwitcherQt::addTask);
}






