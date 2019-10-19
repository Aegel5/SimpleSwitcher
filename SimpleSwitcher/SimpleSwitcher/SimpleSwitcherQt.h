#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SimpleSwitcherQt.h"

class SimpleSwitcherQt : public QMainWindow
{
	Q_OBJECT

public:
	SimpleSwitcherQt(QWidget *parent = Q_NULLPTR);

public slots:
	//void ChangeHotKey(QLineEdit* lineEdit);

private:
	Ui::SimpleSwitcherQtClass ui;
	void ShowChangeHotKey(QLineHotKey* lineEdit);

protected:
	bool eventFilter(QObject *obj, QEvent *ev) override;
};
