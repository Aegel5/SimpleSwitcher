#include "stdafx.h"

#include "PageMainQt.h"
#include <qdebug.h>
#include "Settings.h"

void PageMainQt::enableChanged(int state)
{
	qDebug() << "enableChanged";

	if (SettingsGlobal().isEnabled)
	{
		IFS_LOG(gdata().procMonitor.Stop());
	}
	else
	{
		IFS_LOG(gdata().procMonitor.EnsureStarted(
			SettingsGlobal().isMonitorAdmin ? SW_ADMIN_ON : SW_ADMIN_SELF
		));
	}

	enableUpdate();
}

void PageMainQt::enableUpdate()
{
	TSWCheckRunRes res = gdata().procMonitor.CheckRunning();
	if (res.found)
	{
		if (SettingsGlobal().isMonitorAdmin != res.admin)
		{
			if (res.admin == false)
			{
				IFS_LOG(gdata().procMonitor.Stop());
				res = gdata().procMonitor.CheckRunning();
			}
		}
	}

	SettingsGlobal().isEnabled = res.found;

	LOG_INFO_1(L"Set enabled=%u", SettingsGlobal().isEnabled);
	ui->checkBox_enable->setChecked(SettingsGlobal().isEnabled);
}

void PageMainQt::init(Ui::SimpleSwitcherQtClass * ui)
{
	this->ui = ui;

	connect(ui->checkBox_enable, SIGNAL(stateChanged(int)), this, SLOT(enableChanged(int)));

	enableUpdate();
}
