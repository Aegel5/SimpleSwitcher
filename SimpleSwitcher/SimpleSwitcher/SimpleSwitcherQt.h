#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SimpleSwitcherQt.h"


class SimpleSwitcherQt : public QMainWindow
{
	Q_OBJECT

public:
	static SimpleSwitcherQt* Instance() { return instance; }
	SimpleSwitcherQt(QWidget *parent = Q_NULLPTR);

public slots:
	void LeftPanelSelectChanged();

private:
	Ui::SimpleSwitcherQtClass ui;
	static inline SimpleSwitcherQt* instance;

};
