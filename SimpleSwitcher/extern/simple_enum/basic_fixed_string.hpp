// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#pragma once

#include <iostream>
#include <cstddef>      // for std::size_t
#include <type_traits>  // for std::is_same
#include <simple_enum/detail/stralgo_inplace.hpp>
#include <algorithm>

namespace simple_enum::inline v0_8
  {
namespace concepts
  {
  template<typename T>
  concept string_literal = requires {
    { T::value } -> std::convertible_to<char const *>;
  };

  }  // namespace concepts

template<typename CharT, std::size_t N>
struct basic_fixed_string
  {
  CharT str[N + 1]{};

  constexpr operator CharT const *() const noexcept { return str; }

  constexpr CharT const * data() const noexcept { return str; }

  constexpr auto view() const noexcept { return std::basic_string_view{data(), size()}; }

  constexpr operator std::basic_string_view<CharT>() const noexcept { return view(); }

  static constexpr std::size_t size() noexcept { return N; }

  constexpr auto operator==(basic_fixed_string const & rh) const noexcept -> bool
    {
    return std::equal(data(), std::next(data(), N), rh.data());
    }
  };

template<typename T, std::size_t N>
basic_fixed_string(T const (&)[N]) -> basic_fixed_string<T, N - 1>;

template<auto & S>
struct string_literal
  {
  static constexpr auto & value = S;
  };

namespace detail
  {

  struct camel_case_character_t
    {
    bool make_upper = true;

    template<typename CharT>
      requires std::convertible_to<CharT, char>
    constexpr auto operator()(CharT c) noexcept -> CharT
      {
      if(c == '_' || c == ' ' || c == '\0')
        {
        make_upper = true;
        if(c == '_')
          return ' ';
        else
          return c;
        }
      else if(make_upper)
        {
        make_upper = false;
        return detail::to_upper(c);
        }
      else [[likely]]
        return c;
      }
    };
  }  // namespace detail

template<typename CharT, std::size_t N>
consteval auto as_basic_fixed_string(char const * input) -> basic_fixed_string<CharT, N>
  {
  basic_fixed_string<CharT, N> result{};
  for(std::size_t i = 0; i < N; ++i)
#ifdef __clang__
#pragma clang unsafe_buffer_usage begin
#endif
    result.str[i] = input[i];
#ifdef __clang__
#pragma clang unsafe_buffer_usage end
#endif
  return result;
  }

template<typename CharT, std::size_t N>
consteval auto to_camel_case(basic_fixed_string<CharT, N> const & input) -> basic_fixed_string<CharT, N>
  {
  basic_fixed_string<CharT, N> result{};

  detail::camel_case_character_t camel_case_character;
  for(std::size_t i = 0; i < N; ++i)
#ifdef __clang__
#pragma clang unsafe_buffer_usage begin
#endif
    result.str[i] = camel_case_character(input.str[i]);
#ifdef __clang__
#pragma clang unsafe_buffer_usage end
#endif

  return result;
  }

template<typename CharT>
constexpr auto to_camel_case(std::basic_string_view<CharT> input) -> std::basic_string<CharT>
  {
  std::basic_string<CharT> result{};
  detail::camel_case_character_t camel_case_character;
  result.reserve(input.size());
  std::ranges::transform(input, std::back_inserter(result), camel_case_character);
  return result;
  }
  }  // namespace simple_enum::inline v0_8
