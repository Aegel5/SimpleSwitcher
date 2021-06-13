#pragma once

struct BufScanMap
{
	struct BufScanMapIter
	{
		BufScanMap* parent;
		size_t index;

		DWORD curDWORD()
		{
			auto res = parent->DWORDI(index);
			return res;
		}
		TKeyCode curKeySrc()
		{
			WORD src = HIWORD(curDWORD());
			SHORT virtcode = MapVirtualKey(src, MAPVK_VSC_TO_VK);
			return (TKeyCode)virtcode;
		}
		std::pair<WORD, TKeyCode> curElemSrc()
		{
			WORD src = HIWORD(curDWORD());
			SHORT virtcode = MapVirtualKey(src, MAPVK_VSC_TO_VK);
			return std::make_pair(src, virtcode);
		}
		std::pair<WORD, TKeyCode> curElemDst()
		{
			WORD src = LOWORD(curDWORD());
			SHORT virtcode = MapVirtualKey(src, MAPVK_VSC_TO_VK);
			return std::make_pair(src, virtcode);
		}
		size_t Index()
		{
			return index;
		}
		size_t logicIndex() {
			return index - 3;
		}
		void set(TKeyCode src, TKeyCode val)
		{
			parent->DWORDI(index) = BufScanMap::PackToDWORD(src, val);
		}
		TKeyCode curKeyVal()
		{
			WORD val = LOWORD(curDWORD());
			SHORT virtcode = MapVirtualKey(val, MAPVK_VSC_TO_VK);
			return (TKeyCode)virtcode;
		}
		bool IsEnd()
		{
			size_t indexInBytes = index * sizeof(DWORD);
			if (indexInBytes >= parent->size)
			{
				return true;
			}
			if (curDWORD() == 0)
			{
				return true;
			}

			return false;
		}
		void operator++()
		{
			++index;
		}
	};

	static const DWORD c_size = 128;
	BYTE buf[c_size];
	DWORD size;

	void Clear()
	{
		ZeroMemory(buf, c_size);
		SizeInTable() = 1;
		size = 16;
	}

	BufScanMap()
	{
		Clear();
	}
	BufScanMapIter GetIter()
	{
		BufScanMapIter iter;
		iter.parent = this;
		iter.index = 3;
		return iter;
	}
	TStatus FromRegistry()
	{
		CAutoCloseHKey hg;
		LSTATUS stat = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout",
			0,
			KEY_READ,
			&hg);

		if (stat == ERROR_SUCCESS)
		{
			DWORD type = 0;
			size = c_size;
			LONG res = RegQueryValueEx(hg, L"Scancode Map", 0, &type, buf, &size);
			if (res != 0 || type != REG_BINARY)
			{
				Clear();
			}
			int k = 0;
		}

		RETURN_SUCCESS;
	}
	bool IsEmpty()
	{
		if (size <= 16)
			return true;

		auto iter = GetIter();

		if (iter.IsEnd())
			return true;

		return false;
	}


	void DelRemapKey(TKeyCode keySrc)
	{
		for (auto iter = GetIter(); !iter.IsEnd();)
		{
			if (iter.curKeySrc() == keySrc)
			{
				auto i = iter.Index();
				DelByIndex(i);
			}
			else
			{
				++iter;
			}
		}
	}
	void DelRemapKey_ByIndex(int itodel)
	{
		for (auto iter = GetIter(); !iter.IsEnd();) {
			if (itodel == iter.logicIndex()) {
				DelByIndex(iter.Index());
				return;
			}
			++iter;
		}
	}

	TStatus PutRemapKey(TKeyCode keySrc, TKeyCode keyVal)
	{
		for (auto iter = GetIter(); !iter.IsEnd(); ++iter)
		{
			if (iter.curKeySrc() == keySrc)
			{
				iter.set(keySrc, keyVal);
				RETURN_SUCCESS;
			}
		}

		DWORD dw = PackToDWORD(keySrc, keyVal);
		IFS_RET(AddToEnd(dw));

		RETURN_SUCCESS;
	}
	TStatus PutRemapKey_BySc(DWORD keySrc, DWORD keyVal)
	{
		for (auto iter = GetIter(); !iter.IsEnd(); ++iter)
		{
			// TODO
			//if (iter.curElemSrc().first == keySrc)
			//{
			//	iter.set(keySrc, keyVal);
			//	RETURN_SUCCESS;
			//}
		}

		DWORD dw = MAKELONG(keyVal, keySrc);
		IFS_RET(AddToEnd(dw));

		RETURN_SUCCESS;
	}
	bool GetRemapedKey(TKeyCode keySrc, TKeyCode& keyVal)
	{
		keyVal = keySrc;

		for (auto iter = GetIter(); !iter.IsEnd(); ++iter)
		{
			if (iter.curKeySrc() == keySrc)
			{
				keyVal = iter.curKeyVal();
				return true;
			}
		}

		return false;
	}

	TStatus ToRegistry()
	{

		if (IsEmpty())
		{
			IFS_RET(RemoveRegistry());
			RETURN_SUCCESS;
		}

		CAutoCloseHKey hg;
		IF_LSTATUS_RET(RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout",
			0,
			KEY_ALL_ACCESS,
			&hg));

		IF_LSTATUS_RET(RegSetValueEx(hg, L"Scancode Map", 0, REG_BINARY, buf, size));
		RETURN_SUCCESS;
	}
	static TStatus RemoveRegistry()
	{
		CAutoCloseHKey hg;
		IF_LSTATUS_RET(RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout",
			0,
			KEY_ALL_ACCESS,
			&hg));
		IF_LSTATUS_RET(RegDeleteValue(hg, L"Scancode Map"));
		RETURN_SUCCESS;

	}
	private:
		DWORD& DWORDI(size_t index)
		{
			return *(DWORD*)(buf + index * sizeof(DWORD));
		}
		static DWORD PackToDWORD(TKeyCode src, TKeyCode val)
		{
			UINT v = MapVirtualKey(val, MAPVK_VK_TO_VSC);
			UINT s = MapVirtualKey(src, MAPVK_VK_TO_VSC);

			return MAKELONG(v, s);
		}
		DWORD& SizeInTable()
		{
			return DWORDI(2);
		}
		void DelByIndex(size_t index)
		{
			size_t len = size / sizeof(DWORD);
			SizeInTable() -= 1;
			for (size_t i = index + 1; i < len; ++i)
			{
				DWORDI(i - 1) = DWORDI(i);
			}
			DWORDI(len - 1) = 0;
			size -= sizeof(DWORD);
		}
		TStatus AddToEnd(DWORD dw)
		{
			if (size >= c_size)
			{
				IFS_RET(SW_ERR_BUFFER_TOO_SMALL);
			}
			size_t len = size / sizeof(DWORD);
			DWORDI(len - 1) = dw;
			DWORDI(len) = 0;
			size += sizeof(DWORD);
			SizeInTable() += 1;

			RETURN_SUCCESS;
		}
};

