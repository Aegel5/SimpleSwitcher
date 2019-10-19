#include "stdafx.h"
#include "ChangeHotKeyFrame.h"
#include <QVBoxLayout>

ChangeHotKeyFrame::ChangeHotKeyFrame(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//m_quickWidget = new QQuickWidget(this);
	//m_quickWidget->setSource(QUrl("qrc:/ChangeHotKey.qml"));
	//m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

	//QVBoxLayout *layout = new QVBoxLayout(this);
	//layout->addWidget(m_quickWidget);

	//ui.->addWidget(m_quickWidget, 1, 0);
}

ChangeHotKeyFrame::~ChangeHotKeyFrame()
{
	int k = 0;
}
