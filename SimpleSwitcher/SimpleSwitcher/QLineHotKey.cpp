#include "stdafx.h"
#include "QLineHotKey.h"
#include "qwidget.h"

QLineHotKey::QLineHotKey(QWidget *parent)
	: QLineEdit(parent)
{
	this->setReadOnly(true);
	this->setAlignment(Qt::AlignmentFlag::AlignRight | Qt::AlignVCenter);
}

QLineHotKey::~QLineHotKey()
{
}
