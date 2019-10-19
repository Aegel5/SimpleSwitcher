#include "stdafx.h"
#include "QLineHotKey.h"
#include "qwidget.h"
#include <QMouseEvent>
#include <qdebug.h>
#include "SimpleSwitcherQt.h"
#include "ChangeHotKeyFrame.h"

QLineHotKey::QLineHotKey(QWidget *parent)
	: QLineEdit(parent)
{
	this->setReadOnly(true);
	this->setAlignment(Qt::AlignmentFlag::AlignRight | Qt::AlignVCenter);


}
void QLineHotKey::mouseDoubleClickEvent(QMouseEvent *event)
//bool QLineHotKey::eventFilter(QObject *obj, QEvent *event)
{
	ShowChangeHotKey();
	qDebug() << "MouseButtonDblClick";

}

void QLineHotKey::ShowChangeHotKey()
{
	ChangeHotKeyFrame* wnd = new ChangeHotKeyFrame(SimpleSwitcherQt::Instance());
	wnd->setAttribute(Qt::WA_DeleteOnClose);
	wnd->setModal(true);
	wnd->show();
}

QLineHotKey::~QLineHotKey()
{
}
