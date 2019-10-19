#pragma once

#include <QLineEdit>

class QLineHotKey : public QLineEdit
{
	Q_OBJECT

public:
	QLineHotKey(QWidget *parent);
	~QLineHotKey();
protected:
	//bool eventFilter(QObject *obj, QEvent *ev) override;
	void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
	void ShowChangeHotKey();
};
