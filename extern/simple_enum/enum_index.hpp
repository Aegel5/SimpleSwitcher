// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#pragma once

#include <simple_enum/simple_enum.hpp>
#include <simple_enum/expected.h>
#include "detail/static_call_operator_prolog.h"

namespace simple_enum::inline v0_8
  {
using cxx23::bad_expected_access;
using cxx23::expected;
using cxx23::in_place;
using cxx23::in_place_t;
using cxx23::unexpect;
using cxx23::unexpect_t;
using cxx23::unexpected;

enum struct enum_index_error
  {
  out_of_range
  };

consteval auto adl_enum_bounds(enum_index_error)
  {
  using enum enum_index_error;
  return simple_enum::adl_info{out_of_range, out_of_range};
  }

struct enum_index_t
  {
  template<enum_concept enum_type>
  [[nodiscard]]
  static_call_operator constexpr auto operator()(enum_type value) static_call_operator_const noexcept
    -> cxx23::expected<std::size_t, enum_index_error>
    {
    using enum_meta_info = detail::enum_meta_info_t<enum_type>;
    auto const requested_index{simple_enum::detail::to_underlying(value)};
    if(requested_index >= enum_meta_info::first_index() && requested_index <= enum_meta_info::last_index()) [[likely]]
      return requested_index - enum_meta_info::first_index();
    else
      return cxx23::unexpected{enum_index_error::out_of_range};
    }
  };

/**
 * @brief Provides compile or runtime conversion of enum values to their indices.
 *
 * `enum_index_t` is designed to convert enum values to their corresponding indices within the defined range
 *
 * ## Usage
 * The operator() converts an enumeration value to its index, returning a `expected<std::size_t, enum_index_error>`:
 * - On success, it returns the index of the enumeration value.
 * - On failure, it returns `unexpected{enum_index_error::out_of_range}`, indicating the value is outside the valid
 * range.
 *
 * ## Example
 *
 * @code{.cpp}
 * enum class my_enum { value1, value2, value3 };
 *
 * consteval auto adl_enum_bounds(my_enum)
 *  { return simple_enum::adl_info{my_enum::value1, my_enum::value3}; }
 *
 * auto index = enum_index(my_enum::value2);
 * if(index) {
 *     // Use the index
 * } else {
 *     // Handle the error
 * }
 * @endcode
 */
inline constexpr enum_index_t enum_index;

template<enum_concept auto value>
consteval auto consteval_enum_index() -> std::size_t
  {
  return enum_index(value).or_else([](auto &&) { throw; });
  }

  }  // namespace simple_enum::inline v0_8

#include "detail/static_call_operator_epilog.h"
