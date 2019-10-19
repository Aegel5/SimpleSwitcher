#pragma once

#include <QWidget>
#include <qdialog.h>
#include <QQuickWidget>
#include "ui_ChangeHotKeyFrame.h"

class ChangeHotKeyFrame : public QDialog
{
	Q_OBJECT

public:
	ChangeHotKeyFrame(QWidget *parent = Q_NULLPTR);
	~ChangeHotKeyFrame();

private:
	Ui::ChangeHotKeyFrame ui;
	QQuickWidget* m_quickWidget;
};
