#pragma once

#include <QLineEdit>

class QLineHotKey : public QLineEdit
{
	Q_OBJECT

public:
	QLineHotKey(QWidget *parent);
	~QLineHotKey();
};
