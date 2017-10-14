#pragma once

#include "WinBase.h "


class TSyncVal32
{
private:
	typedef volatile long TSyncType;
	TSyncType m_nValue;
public:
	TSyncVal32()
		: m_nValue(0)
	{}

	TSyncVal32(TSyncType nVal)
		: m_nValue(nVal)
	{}

	TSyncType operator=(const TSyncType& val)
	{
		return InterlockedExchange(&m_nValue, val);
	}

	TSyncType operator++()
	{
		return InterlockedIncrement(&m_nValue);
	}

	TSyncType operator++(int)
	{
		return InterlockedIncrement(&m_nValue) - 1;
	}

	TSyncType operator--()
	{
		return InterlockedDecrement(&m_nValue);
	}

	TSyncType operator--(int)
	{
		return InterlockedDecrement(&m_nValue) + 1;
	}

	operator TSyncType()
	{
		return m_nValue;
	}

	//TSyncType operator+=(const TSyncType& nValue)
	//{ 
	//	return InterlockedAdd(nValue); 
	//}
	////
	//TSyncType operator-=(const TSyncType& nValue)
	//{ 
	//	return InterlockedAdd(0 - nValue); 
	//}
};

class CAutoCounter
{
public:
	CAutoCounter(TSyncVal32& v) : val(v) { ++val; }
	~CAutoCounter(){ --val; }
	TSyncVal32& val;
};

