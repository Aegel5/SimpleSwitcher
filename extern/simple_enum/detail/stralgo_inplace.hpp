// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum

#pragma once

#include "static_call_operator_prolog.h"

namespace simple_enum::inline v0_8::detail
  {

// This is inplace copy of part of my other library https://github.com/arturbac/stralgo

inline constexpr unsigned lower_char_begin_ = 97u;
inline constexpr unsigned upper_char_begin_ = 65u;

///\brief match https://en.cppreference.com/w/cpp/string/byte/islower but is resistant to signed char UB
struct islower_t
  {
  [[nodiscard]]
  static_call_operator constexpr bool operator()(auto src) static_call_operator_const noexcept
    {
    unsigned c{static_cast<unsigned>(src)};
    return c >= lower_char_begin_ && c <= 122u;
    }
  };

inline constexpr islower_t islower;

struct to_upper_t
  {
  template<typename char_type>
  [[nodiscard]]
  static_call_operator constexpr auto operator()(char_type c) static_call_operator_const noexcept -> char_type
    {
    if(islower(c))
      return static_cast<char_type>(static_cast<unsigned>(c) - lower_char_begin_ + upper_char_begin_);
    return c;
    }
  };

inline constexpr to_upper_t to_upper;

  }  // namespace simple_enum::inline v0_8::detail

#include "static_call_operator_epilog.h"
