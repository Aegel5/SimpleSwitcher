#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SimpleSwitcherQt.h"

#include "PageMainQt.h"


class SimpleSwitcherQt : public QMainWindow
{
	Q_OBJECT

public:
	static SimpleSwitcherQt* Instance() { return instance; }
	SimpleSwitcherQt(QWidget *parent = Q_NULLPTR);

public slots:
	void leftPanelSelectChanged();

private:
	Ui::SimpleSwitcherQtClass ui;
	static inline SimpleSwitcherQt* instance;

	// Обработчики страниц
	PageMainQt pageMain;

};
