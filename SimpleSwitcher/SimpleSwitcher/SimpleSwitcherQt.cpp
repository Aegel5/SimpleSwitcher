#include "stdafx.h"
#include "SimpleSwitcherQt.h"


#include <QDebug>


void SimpleSwitcherQt::leftPanelSelectChanged()
{
	qDebug() << "LeftPanelSelectChanged";

	//ui.tabWidget->setCurrentIndex();
}

SimpleSwitcherQt::SimpleSwitcherQt(QWidget *parent)
	: QMainWindow(parent)
{

	ui.setupUi(this);
	pageMain.init(&ui);

	instance = this;


	ui.tabWidget->tabBar()->hide();
	ui.tabWidget->setCurrentIndex(0);

	connect(
		ui.listWidget,
		&QListWidget::itemSelectionChanged,
		this,
		[this]() {ui.tabWidget->setCurrentIndex(ui.listWidget->currentRow()); }
	);

}






