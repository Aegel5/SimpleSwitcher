#pragma once


#define SW_ARRAY_SIZE(V)		(sizeof(V) / sizeof(V[0]))

typedef unsigned __int64 TUInt64;
typedef signed   __int64 TInt64;
typedef unsigned __int32 TUInt32;
typedef signed   __int32 TInt32;
typedef unsigned __int16 TUInt16;
typedef signed   __int16 TInt16;
typedef unsigned __int8  TUInt8;
typedef signed   __int8  TInt8;

enum TSWBit
{
	SW_BIT_32,
	SW_BIT_64,
};

typedef TCHAR TChar;
typedef const TChar* TStr;

typedef char TAChar;
typedef const TAChar* TAStr;

typedef std::wstring tstring;

template <typename T1, typename T2>
inline bool TestFlagAll(T1 V, T2 F)
{
	return (V & F) == F;
}


template <typename T1, typename T2>
inline bool TestFlag(T1 V, T2 F)
{
	return TUInt32(V & F) != 0; // TODO: operators for enum == 0 
}


template <typename T, typename T2>
inline void SetFlag(T& V, T2 F)
{
	V |= F;
}

template <typename T, typename TE>
inline void ResetFlag(T& V, TE F)
{
	V &= (-1 ^ (F));
}

// TODO move to win

enum TSWAdmin
{
	SW_ADMIN_ON,
	SW_ADMIN_OFF, // not use, bugs when UAC enabled.
	SW_ADMIN_SELF,
};

inline bool IsSelf64()
{
#ifdef _WIN64
	return true;
#elif _WIN32
	return false;
#else
	!!ERROR!!
#endif
}

inline TSWBit GetSelfBit()
{
	return IsSelf64() ? SW_BIT_64 : SW_BIT_32;
}


template<class T>
inline void SwZeroMemory(T& t)
{
	ZeroMemory(&t, sizeof(T));
}

