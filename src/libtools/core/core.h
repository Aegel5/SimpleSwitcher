#pragma once

typedef unsigned __int64 TUInt64;
typedef signed   __int64 TInt64;
typedef unsigned __int32 TUInt32;
typedef signed   __int32 TInt32;
typedef unsigned __int16 TUInt16;
typedef signed   __int16 TInt16;
typedef unsigned __int8  TUInt8;
typedef signed   __int8  TInt8;

typedef TCHAR TChar;
typedef const TChar* TStr;
using UStr = const char*;
using SView = std::wstring_view;
using UView = std::string_view;
using std::wstring;
using std::string;
using std::vector;
using namespace std::string_literals;

using SpanByte = std::span<std::byte>;

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


template<class T>
inline void SwZeroMemory(T& t)
{
	ZeroMemory(&t, sizeof(T));
}

#define FORWARD(x) std::forward<decltype(x)>(x)
#define DECLTYPE_DECAY(x) std::decay_t<decltype(x)>
#define MAKE_SHARED(x) std::make_shared<DECLTYPE_DECAY(x)::element_type>()
#define MAKE_UNIQUE(x) std::make_unique<DECLTYPE_DECAY(x)::element_type>()
#define MAX_FOR(x) std::numeric_limits<DECLTYPE_DECAY(x)>::max()

using std::numeric_limits;

inline void Clamp(auto& v, auto l, auto h) {	v = std::clamp(v, (DECLTYPE_DECAY(v))l, (DECLTYPE_DECAY(v))h);}

struct Vec_f2 { float x = 0; float y = 0; };
struct Vec_i2 { int x = 0; int y = 0; };

inline auto RoundToInt(auto v) { return (int)std::round(v); }
