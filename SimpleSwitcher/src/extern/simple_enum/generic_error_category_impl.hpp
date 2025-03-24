// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum

#pragma once

#include <simple_enum/generic_error_category.hpp>
#include <simple_enum/basic_fixed_string.hpp>

namespace simple_enum::inline v0_8
  {

/**
 * @brief Setting custom category name thru std::is_error_code_enum.
 *
 * Because std::is_error_code_enum must be specialized anyway it can be used to set different category name value. By
 * adding static constexpr std::string_view category_name to this specialization , users can provide a constexpr
 * std::string_view that represents a custom category name for their specific error enumeration. When not specialized
 * cateogry name will be deduced form enumeration type name applying Camel space Casing
 *
 * ErrorEnum The error enum class for which the category name is being defined. This type should conform
 *         to the concepts::error_enum concept, ensuring it is an enum suited for error representation.
 *
 * @code
 * enum class MyError {
 *     Error1,
 *     Error2
 * };
 *
 * template<>
 * struct simple_enum::error_category_name<MyError> {
 *     static constexpr std::string_view value = "My Error Category";
 * };
 * @endcode
 */

namespace detail
  {
  // Define the concept error_category_name_specialized
  template<typename ErrorEnum>
  concept error_category_name_specialized = requires {
    { std::is_error_code_enum<ErrorEnum>::category_name } -> std::same_as<std::string_view const &>;
  };

  /*
  // Example usage
  template<error_enum ErrorEnum>
  struct error_category_name<ErrorEnum> {
      static constexpr std::string_view value = "DefaultErrorCategory";
  };

  // A test enum to specialize error_category_name
  enum class MyError {
      ErrorCode1,
      ErrorCode2
  };

  // Specialization for MyError
  template<>
  struct error_category_name<MyError> {
      static constexpr std::string_view value = "MyErrorCategory";
  };

  static_assert(error_category_name_specialized<MyError>, "MyError must specialize error_category_name with a static
  constexpr std::string_view value.");
  */

  template<concepts::error_enum ErrorEnum>
  struct cxx20_generic_error_category_name
    {
    static constexpr auto error_category_name = simple_enum::enumeration_name_v<ErrorEnum>;
    static constexpr size_t error_category_name_len{error_category_name.size()};
    };

  template<concepts::error_enum ErrorEnum>
  consteval auto generic_error_category_name()
    {
    if constexpr(error_category_name_specialized<ErrorEnum>)
      return std::is_error_code_enum<ErrorEnum>::category_name;
    else
      {
      using meta = cxx20_generic_error_category_name<ErrorEnum>;
      return to_camel_case(as_basic_fixed_string<char, meta::error_category_name_len>(meta::error_category_name.data())
      );
      }
    }
  }  // namespace detail

template<concepts::error_enum ErrorEnum>
char const * generic_error_category<ErrorEnum>::name() const noexcept
  {
  static constexpr auto name_{detail::generic_error_category_name<ErrorEnum>()};
  return name_.data();
  }

template<concepts::error_enum ErrorEnum>
std::string generic_error_category<ErrorEnum>::message(int ev) const
  {
  return to_camel_case(enum_name(static_cast<ErrorEnum>(ev)));
  }

template<concepts::error_enum ErrorEnum>
auto generic_error_category<ErrorEnum>::instance() -> generic_error_category<ErrorEnum> const &
  {
  static generic_error_category category
#ifdef __clang__
    [[clang::no_destroy]]
#endif
    ;
  return category;
  }

template<concepts::error_enum ErrorEnum>
auto generic_error_category<ErrorEnum>::make_error_code(ErrorEnum e) noexcept -> std::error_code
  {
  return {static_cast<int>(e), generic_error_category<ErrorEnum>::instance()};
  }
  }  // namespace simple_enum::inline v0_8
