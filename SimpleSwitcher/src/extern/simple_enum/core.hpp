// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#pragma once

#include <concepts>
#include <type_traits>

#define SIMPLE_ENUM_NAME_VERSION "0.8.12"

namespace simple_enum::inline v0_8
  {
inline constexpr auto simple_enum_name_version{SIMPLE_ENUM_NAME_VERSION};

#ifndef SIMPLE_ENUM_CUSTOM_UNBOUNDED_RANGE
#define SIMPLE_ENUM_CUSTOM_UNBOUNDED_RANGE
inline constexpr auto default_unbounded_upper_range = 10;
#endif

template<typename type>
concept enum_concept = std::is_enum_v<type>;

namespace concepts
  {
  template<typename T>
  concept strong_enum = enum_concept<T> && !std::convertible_to<T, std::underlying_type_t<T>>;
  }

/**
 * @brief Struct template designed for user specialization to provide enumeration bounds information.
 *
 * This struct serves as a template for users to define external bounds for enumeration types. By specializing
 * this template, users can supply custom boundary information (first and last values) for specific enumerations,
 * facilitating operations that require knowledge of enumeration range. It is required that the value of `first`
 * is less than or equal to the value of `last`, representing the valid range of the enumeration.
 *
 * Specialization should define two static constexpr members, `first` and `last`, representing the minimum
 * and maximum values of the enumeration, respectively. Both `first` and `last` should be of the enumeration type,
 * and adhere to the condition that `first <= last`.
 *
 * Example of user-defined specialization for `std::memory_order`:
 * @code
 * namespace simple_enum {
 *   template<>
 *   struct info<std::memory_order> {
 *     static constexpr auto first = std::memory_order::relaxed; // Minimum value
 *     static constexpr auto last = std::memory_order::seq_cst;  // Maximum value, ensuring first <= last
 *   };
 * }
 * @endcode
 */
template<typename enumeration>
struct info
  {
  };

template<typename enumeration>
struct adl_info
  {
  enumeration first;
  enumeration last;
  bool error_code_enum = false;

  constexpr adl_info() noexcept = default;

  constexpr adl_info(enumeration f, enumeration l) noexcept : first{f}, last{l}, error_code_enum{false} {}

  constexpr adl_info(enumeration f, enumeration l, bool is_error_code_enum) noexcept :
      first{f},
      last{l},
      error_code_enum{is_error_code_enum}
    {
    }
  };

template<enum_concept enumeration>
adl_info(enumeration const &, enumeration const &) -> adl_info<enumeration>;

/**
 * @brief Function intended for ADL (Argument-Dependent Lookup) to provide custom enumeration bounds.
 *
 * This function is designed to be defined by users who wish to add external enumeration bounds
 * information via ADL.
 * A user-defined function should be placed in same namespace as enumeration and
 * should return an instance of `adl_info` constructed with two elements:
 * the first and last bounds of the enumeration, where both elements are of any `std::integral` type.
 *
 * The `adl_info` struct is templated to work with enumeration types.
 *
 * Example specialization for `my_enum`, assuming `my_enum` satisfies `enum_concept` beiing enumeration:
 * @code
 * consteval auto adl_enum_bounds(my_enum) -> simple_enum::adl_info<my_enum> {
 *     return {my_enum::v1, my_enum::v3};
 * }
 * @endcode
 *
 * Additionally, a template deduction guide is provided for `adl_info` to facilitate its construction
 * with enumeration types that meet the `enum_concept` criteria:
 * @code
 * template<typename enumeration>
 * adl_info(enumeration const &, enumeration const &) -> adl_info<enumeration>;
 * @endcode
 *
 * @note Specializations of this function must ensure that first <= last.
 */
template<typename enumeration>
constexpr auto adl_enum_bounds() -> void;

namespace detail
  {
  template<typename Enum>
  constexpr auto to_underlying(Enum e) noexcept -> std::underlying_type_t<Enum>
    {
    return static_cast<std::underlying_type_t<Enum>>(e);
    }

  template<typename enumeration>
  concept has_valid_adl_enum_bounds = requires(enumeration e) {
    { adl_enum_bounds(e) } -> std::same_as<adl_info<enumeration>>;
  };

  template<typename T>
  concept lower_bounded_enum = requires(T e) {
    requires enum_concept<T>;
      { T::first } -> std::convertible_to<T>;
  };
  template<typename T>
  concept upper_bounded_enum = requires(T e) {
    requires enum_concept<T>;
      { T::last } -> std::convertible_to<T>;
  };

  template<typename T>
  concept internaly_bounded_enum = requires(T e) {
    requires enum_concept<T>;
    requires lower_bounded_enum<T>;
    requires upper_bounded_enum<T>;
    requires(simple_enum::detail::to_underlying(T::last) >= simple_enum::detail::to_underlying(T::first));
  };
  // clang-format off
  template<typename enumeration>
  concept has_info_specialization = requires {
    { info<enumeration>::first } -> std::convertible_to<decltype(info<enumeration>::last)>;
    { info<enumeration>::last } -> std::convertible_to<decltype(info<enumeration>::first)>;

    requires info<enumeration>::first <= info<enumeration>::last;
    };

  // clang-format on
  }  // namespace detail

template<typename enumeration>
concept bounded_enum = detail::has_valid_adl_enum_bounds<enumeration> || detail::has_info_specialization<enumeration>
                       || detail::internaly_bounded_enum<enumeration>;
  }  // namespace simple_enum::inline v0_8
