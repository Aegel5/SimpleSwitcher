// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#pragma once

#include <simple_enum/simple_enum.hpp>
#include <iterator>
#include <ranges>

namespace simple_enum::inline v0_8::detail
  {
template<bounded_enum enumeration>
class enum_view_iterator
  {
public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = enumeration;
  using difference_type = std::ptrdiff_t;
  using pointer = enumeration const *;
  using reference = enumeration const &;

private:
  enumeration current_{};

public:
  constexpr enum_view_iterator() noexcept = default;

  constexpr explicit enum_view_iterator(enumeration current) noexcept : current_(current) {}

  constexpr auto operator*() const noexcept -> reference { return current_; }

  constexpr auto operator->() const noexcept -> pointer { return &current_; }

  constexpr auto operator++() noexcept -> enum_view_iterator &
    {
    current_ = static_cast<enumeration>(simple_enum::detail::to_underlying(current_) + 1);
    return *this;
    }

  constexpr auto operator++(int) noexcept -> enum_view_iterator
    {
    enum_view_iterator tmp = *this;
    ++(*this);
    return tmp;
    }

  constexpr auto operator--() noexcept -> enum_view_iterator &
    {
    current_ = static_cast<enumeration>(simple_enum::detail::to_underlying(current_) - 1);
    return *this;
    }

  constexpr auto operator--(int) noexcept -> enum_view_iterator
    {
    enum_view_iterator tmp = *this;
    --(*this);
    return tmp;
    }

  constexpr auto operator+=(difference_type n) noexcept -> enum_view_iterator &
    {
    current_ = static_cast<enumeration>(simple_enum::detail::to_underlying(current_) + n);
    return *this;
    }

  constexpr auto operator-=(difference_type n) noexcept -> enum_view_iterator &
    {
    current_ = static_cast<enumeration>(simple_enum::detail::to_underlying(current_) - n);
    return *this;
    }

  constexpr auto operator-(enum_view_iterator const & other) const noexcept -> difference_type
    {
    return simple_enum::detail::to_underlying(current_) - simple_enum::detail::to_underlying(other.current_);
    }

  constexpr auto operator[](difference_type n) const noexcept -> reference { return *(*this + n); }

  constexpr auto operator<(enum_view_iterator const & other) const noexcept -> bool
    {
    return simple_enum::detail::to_underlying(current_) < simple_enum::detail::to_underlying(other.current_);
    }

  constexpr auto operator>(enum_view_iterator const & other) const noexcept -> bool
    {
    return simple_enum::detail::to_underlying(current_) > simple_enum::detail::to_underlying(other.current_);
    }

  constexpr auto operator<=(enum_view_iterator const & other) const noexcept -> bool
    {
    return simple_enum::detail::to_underlying(current_) <= simple_enum::detail::to_underlying(other.current_);
    }

  constexpr auto operator>=(enum_view_iterator const & other) const noexcept -> bool
    {
    return simple_enum::detail::to_underlying(current_) >= simple_enum::detail::to_underlying(other.current_);
    }

  constexpr auto operator==(enum_view_iterator const & other) const noexcept -> bool = default;
  };
  }  // namespace simple_enum::inline v0_8::detail

namespace simple_enum::inline v0_8
  {

template<enum_concept enumeration>
class enum_view : public std::ranges::view_interface<enum_view<enumeration>>
  {
  static_assert(std::is_enum_v<enumeration>, "enumeration must be an enum type");

public:
  using iterator = detail::enum_view_iterator<enumeration>;

private:
  enumeration first_;
  enumeration last_;

public:
  constexpr enum_view() noexcept
    requires bounded_enum<enumeration>
      : first_(detail::enum_base_info_t<enumeration>::first()), last_(detail::enum_base_info_t<enumeration>::last())

    {
    }

  constexpr enum_view(enumeration first, enumeration last) noexcept : first_(first), last_(last) {}

  constexpr auto begin() const noexcept -> iterator { return iterator(first_); }

  constexpr auto end() const noexcept -> iterator
    {
    return iterator(static_cast<enumeration>(simple_enum::detail::to_underlying(last_) + 1));
    }
  };

template<typename enumeration>
enum_view(enumeration) -> enum_view<enumeration>;

template<typename enumeration>
enum_view(enumeration, enumeration) -> enum_view<enumeration>;

/**
 * @brief Returns a view over all enumeration values for bounded enumerations.
 *
 * This function object is designed to work specifically with bounded enumerations,
 * which are enumerations where the range of valid values is explicitly defined.
 * It utilizes `enum_view` to provide a range-based view over the entire collection
 * of enumeration values, facilitating iteration and other range-based operations.
 *
 * Usage example:
 * @code
 * enum class Color { Red, Green, Blue, first = Red, last = Blue };
 *
 * auto color_values = enum_enumerations<Color>();
 * for (auto enum_value : color_values) {
 *     std::cout << int(enum_value) << std::endl;
 * }
 * @endcode
 */
template<bounded_enum enumeration>
inline constexpr auto enum_enumerations = []() { return enum_view<enumeration>{}; };

/**
 * @brief Constructs a transform view over all values of a bounded enumeration, converting each to its string
 * representation.
 *
 * This function object creates a view that iterates over all possible values of a bounded enumeration
 * and transforms each value into its corresponding string name. This is facilitated by combining `enum_view` with
 * `std::views::transform` and the `enum_name` function. It is designed for bounded enumerations, which are enumerations
 * with explicitly defined bounds, typically via `first` and `last` enumerators.
 *
 * Example Usage:
 * @code
 * enum class Color { Red, Green, Blue, first = Red, last = Blue };
 *
 * // Assuming enum_name is a function that converts Color values to their string representations
 * auto color_names{ enum_names<Color>() };
 * for (auto color_name : color_names) {
 *     std::cout << color_name << std::endl; // Outputs the string representation of each Color value
 * }
 * @endcode
 */
template<bounded_enum enumeration>
inline constexpr auto enum_names = []() { return enum_view<enumeration>{} | std::views::transform(enum_name); };

template<typename enumeration>
constexpr auto begin(enum_view<enumeration> const & v) -> typename enum_view<enumeration>::iterator
  {
  return v.begin();
  }

template<typename enumeration>
constexpr auto end(enum_view<enumeration> const & v) -> typename enum_view<enumeration>::iterator
  {
  return v.end();
  }

  }  // namespace simple_enum::inline v0_8

namespace std::ranges
  {

template<typename enumeration>
inline constexpr bool enable_borrowed_range<simple_enum::enum_view<enumeration>> = true;

  }  // namespace std::ranges
