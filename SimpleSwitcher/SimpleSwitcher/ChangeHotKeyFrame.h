#pragma once

#include <QWidget>
#include <QQuickWidget>
#include "ui_ChangeHotKeyFrame.h"

class ChangeHotKeyFrame : public QWidget
{
	Q_OBJECT

public:
	ChangeHotKeyFrame(QWidget *parent = Q_NULLPTR);
	~ChangeHotKeyFrame();

private:
	Ui::ChangeHotKeyFrame ui;
	QQuickWidget* m_quickWidget;
};
