// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#pragma once
#include <simple_enum/core.hpp>

#include <utility>
#include <cstdint>
#include <string_view>
#if defined(__cpp_lib_source_location)
#include <source_location>
#endif
#include <array>
#include "detail/static_call_operator_prolog.h"

namespace simple_enum::inline v0_8
  {
namespace detail
  {
  struct meta_info_bounds_traits
    {
    bool lower_bound;
    bool upper_bound;
    };

  template<typename enumeration, meta_info_bounds_traits bi = meta_info_bounds_traits{}>
  struct meta_info : std::false_type
    {
    static constexpr enumeration first = static_cast<enumeration>(0);
    static constexpr enumeration last = static_cast<enumeration>(default_unbounded_upper_range);
    };

  template<typename enumeration>
  struct meta_info<enumeration, meta_info_bounds_traits{.lower_bound = true, .upper_bound = true}>
    {
    static constexpr enumeration first = enumeration::first;
    static constexpr enumeration last = enumeration::last;
    };

  // default assume range 0 - 10
  template<typename enumeration>
  struct meta_info<enumeration, meta_info_bounds_traits{.lower_bound = true, .upper_bound = false}>
    {
    static constexpr enumeration first = enumeration::first;
    static constexpr enumeration last = static_cast<enumeration>(default_unbounded_upper_range);
    };

  template<typename enumeration>
  struct meta_info<enumeration, meta_info_bounds_traits{.lower_bound = false, .upper_bound = true}>
    {
    static constexpr enumeration first = static_cast<enumeration>(0);
    static constexpr enumeration last = enumeration::last;
    };

  // strange compile time error on msvc when meta_info_bounds_traits are used directly in template arguments
  template<typename enumeration>
  struct msvc_meta_info_wrapper
    {
    static constexpr meta_info_bounds_traits traits{
      .lower_bound = lower_bounded_enum<enumeration>, .upper_bound = upper_bounded_enum<enumeration>
    };
    using type = meta_info<enumeration, traits>;
    };

  template<typename enumeration>
  consteval auto get_meta_info()
    {
    if constexpr(detail::has_valid_adl_enum_bounds<enumeration>)
      return adl_enum_bounds(enumeration{});
    else if constexpr(has_info_specialization<enumeration>)
      return info<enumeration>{};
    else
      return typename msvc_meta_info_wrapper<enumeration>::type{};
    }
  }  // namespace detail
  }  // namespace simple_enum::inline v0_8

// this namespace is for reducing time crunching source location and .text data
namespace se
  {
template<auto enumeration>
constexpr auto f() noexcept
  {
#if defined(__clang__) || defined(__GNUC__)
  char const * const func{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
  char const * const func{__FUNCSIG__};
#elif defined(__cpp_lib_source_location)
  char const * const func{std::source_location::current().function_name()};
#else
#error "Can not help you not a clang, gcc or msvc"
#endif
  return func;
  }

#if defined(__clang__)
#if __clang_major__ < 13
#error "Clang minimal supported version is 13"
#endif
// This is just for speeding up user use cases to skip computation of strlen at compile time

// index of the = character in the given line "auto se::f() [enumeration =" is 26
inline constexpr auto initial_offset{26 + 1};
inline constexpr auto end_of_enumeration_name = ']';

#elif defined(__GNUC__)
#if __GNUC__ < 10
#error "Gcc minimal supported version is 10"
#endif
// index of the = character in the given line "constexpr auto se::f() [with auto enumeration =" is 46.
inline constexpr auto initial_offset{46 + 1};
inline constexpr char end_of_enumeration_name = ']';

#elif defined(_MSC_VER)
// index of the < character in the given line "auto __cdecl se::f<" is 18
//                         0x00007ff76b5d33b0 "auto __cdecl se::f<v1>(void) noexcept"
inline constexpr auto initial_offset{18};
inline constexpr char end_of_enumeration_name = '>';
#else
#error "supply information to author about Your compiler"
#endif

#ifdef SIMPLE_ENUM_OPT_IN_STATIC_ASSERTS
// OPT IN TESTING CODE
enum struct verify_ennum_
  {
  v1
  };

constexpr size_t find_enumeration_offset()
  {
  auto const func{std::string_view{f<verify_ennum_::v1>()}};
#if defined(_MSC_VER)
  size_t pos = func.find('<');
  if(pos == std::string_view::npos)
    throw;
  return pos;
#else
  size_t pos = func.find("enumeration =");
  if(pos == std::string_view::npos)
    throw;
  return pos + 12 + 1;
#endif
  }

auto constexpr verify_offset() -> bool { return find_enumeration_offset() == initial_offset; }

static_assert(verify_offset());
#endif
  }  // namespace se

namespace simple_enum::inline v0_8
  {
namespace detail
  {
  struct meta_name
    {
    char const * data;
    size_t size;

    constexpr operator std::string_view() const noexcept { return std::string_view{data, size}; }

    constexpr auto as_view() const noexcept -> std::string_view { return std::string_view{data, size}; }

    constexpr auto operator==(meta_name r) const noexcept -> bool { return as_view() == r.as_view(); }

    constexpr auto operator<=>(meta_name r) const noexcept { return as_view() <=> r.as_view(); }
    };

  struct meta_enum_name
    {
    meta_name enum_name;
    meta_name enumeration_name;
    };

  constexpr char const * find_sentinel(char const * str)
    {
#ifdef __clang__
#pragma clang unsafe_buffer_usage begin
#endif
    while(*str != ':' && *str != ')')
      ++str;
#ifdef __clang__
#pragma clang unsafe_buffer_usage end
#endif
    return str;
    }

  template<char end_of_enum = ']'>
  constexpr void parse_enumeration_name(char const * str, meta_name & result) noexcept
    {
#ifdef __clang__
#pragma clang unsafe_buffer_usage begin
#endif
    if(*str == '(')
      ++str;
    char const * prev_colon = str;
    char const * current_colon = str;

    while(*str != end_of_enum)
      {
      if(*str == ':')
        {
        prev_colon = current_colon;
        current_colon = str;
        str += 2;
        continue;
        }
      else if(*str == '(')
        {
        prev_colon = current_colon;
        current_colon = str;
        str += 1;
        continue;
        }
#if defined(_MSC_VER) || defined(SIMPLE_ENNUM_ENABLE_PEN_TEST)
      else if(*str == ' ')  // msvc
        {
        prev_colon = current_colon;
        current_colon = str;
        str += 1;
        continue;
        }
#endif
      else if(*str == ')')
        {
        prev_colon = current_colon;
        current_colon = str;
        str += 1;
        if(*str == ':')
          {
          str += 2;
          continue;
          }
        continue;
        }
      ++str;
      }

    if(*prev_colon == ':')
      prev_colon += 2;
    else if(*prev_colon == '(')
      ++prev_colon;
#if defined(_MSC_VER) || defined(SIMPLE_ENNUM_ENABLE_PEN_TEST)
    else if(*prev_colon == ' ')  // msvc
      ++prev_colon;
#endif
    else if(prev_colon[0] == ')' && prev_colon[1] == ':')
      prev_colon += 3;
#ifdef __clang__
#pragma clang unsafe_buffer_usage end
#endif
    // Calculate the size and set the result
    result.data = prev_colon;
    result.size = size_t(current_colon - prev_colon);
    }

  template<concepts::strong_enum enumeration>
  constexpr meta_name parse_enumeration_name() noexcept
    {
    char const * const func{se::f<enumeration{}>()};
    meta_name result;
#ifdef __clang__
#pragma clang unsafe_buffer_usage begin
#endif
    parse_enumeration_name<se::end_of_enumeration_name>(func + se::initial_offset + 1, result);
#ifdef __clang__
#pragma clang unsafe_buffer_usage end
#endif
    return result;
    }

  template<auto enumeration>
  constexpr auto first_pass(meta_name & res) noexcept
    {
#ifdef __clang__
#pragma clang unsafe_buffer_usage begin
#endif
    char const * const func{se::f<enumeration>()};
    char const * end_of_name{func + se::initial_offset};
    char const * last_colon{end_of_name};
#ifdef _MSC_VER
    size_t was_undefined{};
#endif
    for(; *end_of_name != se::end_of_enumeration_name; ++end_of_name)
#ifdef _MSC_VER
      if(*end_of_name == ':')
        last_colon = end_of_name;
      else if(*end_of_name == ')')
        {
        last_colon = end_of_name;
        was_undefined = 5;
        }
#else
      if(*end_of_name == ':' || *end_of_name == ')')
        last_colon = end_of_name;
#endif

    res.data = last_colon + 1;
    res.size = size_t(end_of_name - res.data);
#ifdef __clang__
#pragma clang unsafe_buffer_usage end
#endif
#ifdef _MSC_VER
    return size_t(last_colon - func) + 1 - was_undefined;
#else
    return size_t(last_colon - func) + 1;
#endif
    }

  template<auto enumeration>
  constexpr void cont_pass(meta_name & res, std::size_t enum_beg) noexcept
    {
#ifdef __clang__
#pragma clang unsafe_buffer_usage begin
#endif
    char const * const func{se::f<enumeration>()};
    char const * end_of_name{func + enum_beg};
    char const * enumeration_name{end_of_name};
    while(*end_of_name != se::end_of_enumeration_name)
      ++end_of_name;  // for other enumerations we only need to find end of string
#ifdef __clang__
#pragma clang unsafe_buffer_usage end
#endif
    res.data = enumeration_name;
    res.size = size_t(end_of_name - res.data);
    }

  template<typename enum_type, std::integral auto first, std::size_t size, typename name_array, std::size_t... indices>
  constexpr void apply_meta_enum(name_array & meta, size_t enum_beg, std::index_sequence<indices...>)
    {
    // unpack and call cont_pass for each index, using fold expression
    using utype = std::underlying_type_t<enum_type>;
    (..., cont_pass<static_cast<enum_type>(first + utype(indices))>(meta[indices + 1], enum_beg));
    }

  template<enum_concept enum_type, std::integral auto first_index, std::integral auto last_index>
  consteval auto prepare_enum_meta_info() noexcept -> std::array<detail::meta_name, last_index - first_index + 1>
    {
    constexpr std::size_t size_{last_index - first_index + 1};
    std::array<detail::meta_name, size_> meta;
    size_t enum_beg{first_pass<static_cast<enum_type>(first_index)>(meta[0])};
    if constexpr(size_ > 1)
      apply_meta_enum<enum_type, first_index + 1, size_ - 1>(meta, enum_beg, std::make_index_sequence<size_ - 1>{});
    return meta;
    }

  template<enum_concept enum_type>
  struct enum_base_info_t
    {
    static constexpr auto emum_info_ = get_meta_info<enum_type>();

    static constexpr auto first() noexcept { return emum_info_.first; }

    static constexpr auto last() noexcept { return emum_info_.last; }

    static constexpr auto first_index() noexcept { return simple_enum::detail::to_underlying(emum_info_.first); }

    static constexpr auto last_index() noexcept { return simple_enum::detail::to_underlying(emum_info_.last); }

    static constexpr auto size() noexcept -> std::size_t { return last_index() - first_index() + 1; }
    };

  template<enum_concept enum_type>
  struct enum_meta_info_t : public enum_base_info_t<enum_type>
    {
    using base = enum_base_info_t<enum_type>;

    static constexpr auto meta_data{detail::prepare_enum_meta_info<enum_type, base::first_index(), base::last_index()>(
    )};
    };

  }  // namespace detail

struct enum_name_t
  {
  template<enum_concept enum_type>
  static_call_operator constexpr auto operator()(enum_type value) static_call_operator_const noexcept
    -> std::string_view
    {
    using enum_meta_info = detail::enum_meta_info_t<enum_type>;
    auto const requested_index{simple_enum::detail::to_underlying(value)};
    if(requested_index >= enum_meta_info::first_index() && requested_index <= enum_meta_info::last_index())
      {
      detail::meta_name const & res{enum_meta_info::meta_data[size_t(requested_index - enum_meta_info::first_index())]};
      return std::string_view{res.data, res.size};
      }
    else
      return {""};  // return empty but null terminated
    }
  };

/**
 * @brief Converts enumeration values to string names at compile or runtime.
 *
 * `enum_name_t` allows for conversion of enum values to their string representations by utilizing metadata
 * defined during compile time evaluation
 * @code{.cpp}
 * enum class my_enum { value1, value2, value3 };
 *
 * consteval auto adl_enum_bounds(my_enum)
 *  { return simple_enum::adl_info{my_enum::value1, my_enum::value3}; }
 *
 * std::cout << enum_name(my_enum::value2);
 * @endcode
 */
inline constexpr enum_name_t enum_name;

namespace detail
  {

  template<concepts::strong_enum enum_type>
  struct enumeration_name_t
    {
    static constexpr std::string_view value{detail::parse_enumeration_name<enum_type>()};
    };
  }  // namespace detail

/**
 * @brief Provides the enumeration name for a specified enum type.
 *
 * @details This template deduces and provides access to the name of the enumeration specified by the enum_type
 * parameter, utilizing the compile-time parsing utilities. Usage example:
 *
 * enum class MyEnum { Value1, Value2 };
 * std::cout << enumeration_name_v<MyEnum> << std::endl;
 *
 * This will output the name of the enumeration (e.g., "MyEnum") as a compile-time
 * constant std::string_view.
 */
template<concepts::strong_enum enum_type>
inline constexpr std::string_view enumeration_name_v = detail::enumeration_name_t<enum_type>::value;

namespace detail
  {
  template<bounded_enum enumeration>
  struct min_t
    {
    consteval auto operator()() const -> enumeration { return detail::enum_base_info_t<enumeration>::first(); }
    };

  template<bounded_enum enumeration>
  struct max_t
    {
    consteval auto operator()() const -> enumeration { return detail::enum_base_info_t<enumeration>::last(); }
    };
  }  // namespace detail

namespace limits
  {
  /*!
   * \brief Function object for getting the minimum enum value.
   */
  template<bounded_enum enumeration>
  inline constexpr detail::min_t<enumeration> min{};

  /*!
   * \brief Function object for getting the maximum enum value.
   */
  template<bounded_enum enumeration>
  inline constexpr detail::max_t<enumeration> max{};
  }  // namespace limits

  }  // namespace simple_enum::inline v0_8

#include "detail/static_call_operator_epilog.h"

