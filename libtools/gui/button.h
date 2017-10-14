#pragma  once

namespace gui_tools
{
	inline bool HandleInit(Wmsg& wmsg, GuiHandler pressed)
	{
		if (wmsg.msg == WM_INITDIALOG)
		{
			IFS_LOG(pressed(wmsg.hwnd));
			return true;
		}
		return false;
	}
	inline bool HandleButton(Wmsg& wmsg, UINT idButton, GuiHandler pressed)
	{
		if (wmsg.msg == WM_COMMAND)
		{
			if (LOWORD(wmsg.wparm) == idButton)
			{
				IFS_LOG(pressed(wmsg.hwnd));
				return true;
			}
		}
		return false;
	}
}