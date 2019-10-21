#pragma once
#include "ui_SimpleSwitcherQt.h"


class PageMainQt : public QObject
{
	Q_OBJECT

public slots:
	void enableChanged(int state);
	void enableUpdate();

public:
	void init(Ui::SimpleSwitcherQtClass* ui);
private:
	Ui::SimpleSwitcherQtClass* ui;

};
