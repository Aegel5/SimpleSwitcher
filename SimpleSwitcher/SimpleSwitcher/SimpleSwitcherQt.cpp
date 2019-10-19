#include "stdafx.h"
#include "SimpleSwitcherQt.h"
#include <QMouseEvent>

#include <QDebug>
#include "ChangeHotKeyFrame.h"

//void SimpleSwitcherQt::ChangeHotKey(QLineEdit* lineEdit)
//{
//	qDebug() << "User clicked on the button!";
//}

SimpleSwitcherQt::SimpleSwitcherQt(QWidget *parent)
	: QMainWindow(parent)
{

	ui.setupUi(this);

	auto res = this->findChildren<QLineHotKey*>();

	for (auto obj : res)
	{
		obj->installEventFilter(this);
	}


	//connect(ui.lineEdit_lastword, &QLineEdit::mouseDoubleClickEvent, this, &SimpleSwitcherQt::addTask);
}



void SimpleSwitcherQt::ShowChangeHotKey(QLineHotKey * lineEdit)
{
	ChangeHotKeyFrame* wnd = new ChangeHotKeyFrame(this);
	wnd->setAttribute(Qt::WA_DeleteOnClose);
	wnd->setWindowModality(Qt::ApplicationModal);
	wnd->show();
}

bool SimpleSwitcherQt::eventFilter(QObject *obj, QEvent *event)
{
	QLineHotKey *objLineEdit = qobject_cast<QLineHotKey *>(obj);
	if (objLineEdit != nullptr)
	{
		if (event->type() == QEvent::MouseButtonDblClick) 
		{
			QMouseEvent * mouse_event = static_cast<QMouseEvent *>(event);

			if (mouse_event->MouseButtonDblClick)
			{
				ShowChangeHotKey(objLineEdit);
				qDebug() << "MouseButtonDblClick";
			}
			return true;
		}
		else 
		{
			return false;
		}
	}
	else 
	{
		// pass the event on to the parent class
		return QMainWindow::eventFilter(obj, event);
	}
}
