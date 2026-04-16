#pragma once

typedef wchar_t TChar;
typedef const TChar* TStr;
using UStr = const char*;
using SView = std::wstring_view;
using UView = std::string_view;
using std::wstring;
using std::string;
using std::vector;
using namespace std::string_literals;
using namespace std::chrono_literals;

using SpanByte = std::span<std::byte>;

template <typename T1, typename T2>
inline bool TestFlagAll(T1 V, T2 F)
{
	return (V & F) == F;
}


template <typename T1, typename T2>
inline bool TestFlag(T1 V, T2 F)
{
	return uint32_t(V & F) != 0; // TODO: operators for enum == 0 
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


template<class T>
inline void SwZeroMemory(T& t)
{
	ZeroMemory(&t, sizeof(T));
}

#define FORWARD(x) static_cast<decltype(x)&&>(x)
#define TYPE_OF(x) std::remove_cvref_t<decltype(x)>
#define MAKE_SHARED(x) std::make_shared<TYPE_OF(x)::element_type>()
#define MAKE_UNIQUE(x) std::make_unique<TYPE_OF(x)::element_type>()
//#define MAX_FOR(x) std::numeric_limits<TYPE_OF(x)>::max()

using std::numeric_limits;

inline void Clamp(auto& v, auto l, auto h) {	v = std::clamp(v, (TYPE_OF(v))l, (TYPE_OF(v))h);}

struct Vec_f2 { float x = 0; float y = 0; };
struct Vec_i2 { int x = 0; int y = 0; };

inline auto RoundToInt(auto v) { return (int)std::round(v); }
