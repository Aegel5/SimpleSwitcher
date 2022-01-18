#include "stdafx.h"
#include "KeyTools.h"


//static TSyncVal32 fOur = 0;
//
//bool IsOurInput()
//{
//	return fOur != 0;
//}
TStatus SendOurInput(InputSender& sender)
{
	//fOur = 1;
	auto res = sender.Send();
	//fOur = 0;

	IFS_RET(res);

	RETURN_SUCCESS;
}
TStatus FunnyMoving(TKeyRevert& keylist)
{
	InputSender inputSender;
	CHotKey left(VK_LEFT);
	CHotKey backspace(VK_BACK);
	CHotKey r(VK_RIGHT);
	for (CHotKey& key : keylist)
	{
		IFS_RET(inputSender.AddPress(left));
	}
	for (CHotKey& key : keylist)
	{
		IFS_RET(inputSender.AddPress(backspace));
	}
	for (CHotKey& key : keylist)
	{
		IFS_RET(inputSender.AddPress(r));
	}

	IFS_RET(SendOurInput(inputSender));


	RETURN_SUCCESS;
}
TStatus SendBacks(TUInt32 count)
{
	InputSender inputSender;
	CHotKey backspace(VK_BACK);
	for (TUInt32 i = 0; i < count; ++i)
	{
		IFS_RET(inputSender.AddPress(backspace));
	}

	IFS_RET(SendOurInput(inputSender));


	RETURN_SUCCESS;
}

TStatus SendDels(TUInt32 count)
{
	InputSender inputSender;
	CHotKey left(VK_LEFT);
	CHotKey del(VK_DELETE);

	for (TUInt32 i = 0; i < count; ++i)
	{
		IFS_RET(inputSender.AddPress(left));
	}

	for (TUInt32 i = 0; i < count; ++i)
	{
		IFS_RET(inputSender.AddPress(del));
	}

	IFS_RET(SendOurInput(inputSender));


	RETURN_SUCCESS;
}

TStatus SendKeys(TKeyRevert& keylist)
{
	InputSender inputSender;

	LOG_INFO_1(L"Send %u keys", keylist.size());

	for (CHotKey& key : keylist)
	{
		IFS_RET(inputSender.AddPress(key));
	}

	IFS_RET(SendOurInput(inputSender));


	RETURN_SUCCESS;
}

