// SPDX-FileCopyrightText: 2025 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#pragma once
#include <simple_enum/simple_enum.hpp>

namespace simple_enum::inline v0_8
  {

template<bounded_enum enumeration_type>
constexpr auto enum_size() -> size_t
  {
  return detail::enum_base_info_t<enumeration_type>::size();
  }

template<bounded_enum enumeration_type>
constexpr auto enum_value_at_index(size_t ix) -> enumeration_type
  {
  using underlying_type = std::underlying_type_t<enumeration_type>;
  return static_cast<enumeration_type>(detail::enum_base_info_t<enumeration_type>::first_index() + underlying_type(ix));
  }

template<bounded_enum enumeration_type>
constexpr auto enum_name_at_index(size_t ix) -> std::string_view
  {
  return detail::enum_meta_info_t<enumeration_type>::meta_data[ix].as_view();
  }
  }  // namespace simple_enum::inline v0_8
