// SPDX-FileCopyrightText: 2024-2025 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum

#pragma once

#if defined(SMALL_VECTORS_EXPECTED) && (!defined(SMALL_VECTORS_EXPECTED_API) || SMALL_VECTORS_EXPECTED_API != 2)
#error "mixing different expected implementations in single TU"
#endif

#ifndef SMALL_VECTORS_EXPECTED
#define SMALL_VECTORS_EXPECTED
#define SMALL_VECTORS_EXPECTED_API 2

#include <version>

#if !defined(SMALL_VECTORS_ENABLE_CUSTOM_EXCPECTED) && defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
#include <expected>

namespace cxx23
  {
using std::bad_expected_access;
using std::expected;
using std::in_place;
using std::in_place_t;
using std::unexpect;
using std::unexpect_t;
using std::unexpected;
  }  // namespace cxx23

#else

#include <utility>
#include <concepts>
#include <type_traits>
#include <memory>
#include <cassert>
#include <functional>

namespace cxx23
  {
template<typename T, typename E>
class expected;
template<typename E>
class unexpected;

struct unexpect_t
  {
  explicit unexpect_t() noexcept = default;
  };

inline constexpr unexpect_t unexpect;

using std::in_place;
using std::in_place_t;

namespace concepts
  {
  template<typename T, typename V>
  concept not_same_as = !std::same_as<T, V>;

  template<typename T>
  concept is_unexpected = requires {
    typename T::error_type;
    requires std::same_as<T, unexpected<typename T::error_type>>;
  };

  template<typename E>
  concept unexpected_constraint = requires {
    requires !std::is_array_v<E>;
    requires std::is_object_v<E>;
    requires !is_unexpected<E>;
    requires !std::is_const_v<E>;
    requires !std::is_volatile_v<E>;
    requires std::is_destructible_v<E>;
  };

  template<typename T>
  concept is_expected = requires {
    typename T::value_type;
    typename T::error_type;
    requires std::same_as<T, expected<typename T::value_type, typename T::error_type>>;
  };

  template<typename T>
  concept not_expected = !is_expected<T>;

  template<typename T>
  concept expected_constraint = requires {
    requires !std::is_array_v<T>;
    requires !std::is_reference_v<T>;
    requires !std::is_function_v<T>;
    requires std::is_destructible_v<T> || std::is_void_v<std::remove_cv_t<T>>;
    requires !is_unexpected<T>;
    requires !std::same_as<T, std::in_place_t>;
    requires !std::same_as<T, unexpect_t>;
  };

  template<typename T, typename E>
  concept swap_constraints = requires {
    requires std::is_swappable_v<T> || std::is_void_v<T>;
    requires std::is_swappable_v<E>;
    requires std::is_move_constructible_v<T> || std::is_void_v<T>;
    requires std::is_move_constructible_v<E>;
    requires std::is_nothrow_move_constructible_v<T> || std::is_void_v<T> || std::is_nothrow_move_constructible_v<E>;
  };
  }  // namespace concepts

namespace detail
  {
  template<typename T, typename E>
  inline constexpr bool swap_no_throw
    = (std::is_nothrow_move_constructible_v<T> || std::is_void_v<T>) && std::is_nothrow_move_constructible_v<E>
      && (std::is_nothrow_swappable_v<T> || std::is_void_v<T>) && std::is_nothrow_swappable_v<E>;

  template<bool use_noexcept, typename T>
  struct revert_if_except_t
    {
    struct empty_t
      {
      };

    T value;
    std::conditional_t<use_noexcept, empty_t, T *> release_address;

    constexpr explicit revert_if_except_t(T && v, T * release_addr) : value{std::move(v)}
      {
      if constexpr(!use_noexcept)
        release_address = release_addr;
      }

    constexpr ~revert_if_except_t()
      requires use_noexcept
    = default;

    constexpr T && release() noexcept
      {
      if constexpr(!use_noexcept)
        release_address = nullptr;
      return std::move(value);
      }

    constexpr ~revert_if_except_t()
      {
      if constexpr(!use_noexcept)
        if(release_address != nullptr)
          std::construct_at(release_address, std::move(value));
      }
    };
  }  // namespace detail

template<typename E>
class [[clang::trivial_abi]] unexpected
  {
public:
  static_assert(concepts::unexpected_constraint<E>, "not a valid type for unexpected error type");
  using error_type = E;

private:
  error_type error_;

public:
  constexpr unexpected(unexpected const &) noexcept(std::is_nothrow_copy_constructible_v<error_type>) = default;

  constexpr unexpected(unexpected &&) noexcept(std::is_nothrow_move_constructible_v<error_type>) = default;

  template<typename Err = E>
    requires requires {
      requires concepts::not_expected<std::remove_cvref_t<Err>>;
      requires concepts::not_same_as<std::remove_cvref_t<Err>, std::in_place_t>;
      requires std::constructible_from<error_type, Err>;
    }
  constexpr explicit unexpected(Err && e) noexcept(noexcept(std::forward<Err>(e))) : error_{std::forward<Err>(e)}
    {
    }

  template<typename... Args>
    requires std::constructible_from<E, Args...>
  constexpr explicit unexpected(std::in_place_t, Args &&... args) noexcept(noexcept(std::forward<Args...>(args...))) :
      error_{std::forward<Args>(args)...}
    {
    }

  template<typename U, typename... Args>
  constexpr explicit unexpected(std::in_place_t, std::initializer_list<U> il, Args &&... args) :
      error_{il, std::forward<Args>(args)...}
    {
    }

  constexpr auto error() const & noexcept -> error_type const & { return error_; }

  constexpr auto error() & noexcept -> error_type & { return error_; }

  constexpr auto error() const && noexcept -> error_type const && { return std::move(error_); }

  constexpr auto error() && noexcept -> error_type && { return std::move(error_); }

  constexpr void swap(unexpected & other) noexcept(std::is_nothrow_swappable_v<error_type>)
    requires std::swappable<error_type>
    {
    std::swap(error_, other.error_);
    }

  template<std::equality_comparable_with<error_type> E2>
  friend constexpr bool
    operator==(unexpected const & x, unexpected<E2> const & y) noexcept(noexcept(x.error() == y.error()))
    {
    return x.error() == y.error();
    }

  friend constexpr void swap(unexpected & x, unexpected & y) noexcept(std::is_nothrow_swappable_v<error_type>)
    requires std::swappable<error_type>
    {
    x.swap(y);
    }
  };

template<typename E>
unexpected(E) -> unexpected<E>;

template<typename E>
class bad_expected_access;

template<>
class bad_expected_access<void> : public std::exception
  {
protected:
  bad_expected_access() = default;
  bad_expected_access(bad_expected_access const &) noexcept = default;
  bad_expected_access(bad_expected_access &&) noexcept = default;
  ~bad_expected_access() override = default;

  bad_expected_access & operator=(bad_expected_access const & r) noexcept = default;
  bad_expected_access & operator=(bad_expected_access && r) noexcept = default;

public:
  [[nodiscard]]
  char const * what() const noexcept override
#ifdef SMALL_VECTORS_EXPECTED_VTABLE_INSTANTATION
    ;
#else
    {
    return "access to expected value without value";
    }
#endif
  };

template<typename E>
class bad_expected_access : public bad_expected_access<void>
  {
public:
  using error_type = E;

  error_type error_;

  explicit bad_expected_access(E e) : error_{std::move(e)} {}

  [[nodiscard]]
  auto error() const & noexcept -> error_type const &
    {
    return error_;
    }

  [[nodiscard]]
  auto error() & noexcept -> error_type &
    {
    return error_;
    }

  [[nodiscard]]
  auto error() const && noexcept -> error_type const &&
    {
    return std::move(error_);
    }

  [[nodiscard]]
  auto error() && noexcept -> error_type &&
    {
    return std::move(error_);
    }
  };

namespace detail
  {
  template<typename EX, typename F>
  constexpr auto and_then(EX && ex, F && f);

  template<typename EX, typename F>
  constexpr auto transform(EX && ex, F && f);

  template<typename EX, typename F>
  constexpr auto or_else(EX && ex, F && f);

  template<typename EX, typename F>
  constexpr auto transform_error(EX && ex, F && f);

  template<concepts::is_expected EX>
  inline constexpr void swap_dispatch(EX & l, EX & r);

  template<typename E, typename T>
  struct expected_storage_t
    {
      union {
      T value;
      E error;
      };
    };

  template<typename E>
  struct expected_storage_t<E, void>
    {
      union {
      E error;
      };
    };

  struct swap_expected_t;

  }  // namespace detail

namespace concepts
  {
  template<typename T, typename E, typename U, typename G>
  concept expected_conv_constr = requires {
    requires !std::is_same_v<T, bool>;
    requires !std::is_constructible_v<T, expected<U, G> &>;
    requires !std::is_constructible_v<T, expected<U, G>>;
    requires !std::is_constructible_v<T, expected<U, G> const &>;
    requires !std::is_constructible_v<T, expected<U, G> const>;
    requires !std::is_convertible_v<expected<U, G> &, T>;
    requires !std::is_convertible_v<expected<U, G>, T>;
    requires !std::is_convertible_v<expected<U, G> const &, T>;
    requires !std::is_convertible_v<expected<U, G> const, T>;
    requires !std::is_constructible_v<unexpected<E>, expected<U, G> &>;
    requires !std::is_constructible_v<unexpected<E>, expected<U, G>>;
    requires !std::is_constructible_v<unexpected<E>, expected<U, G> const &>;
    requires !std::is_constructible_v<unexpected<E>, expected<U, G> const>;
  };
  }  // namespace concepts

// https://clang.llvm.org/docs/AttributeReference.html#trivial-abi
// Attribute trivial_abi has no effect when the class has a non-static data member whose type is non-trivial for the
// purposes of calls
template<typename T, typename E>
class [[nodiscard, clang::trivial_abi]] expected
  {
public:
  static_assert(concepts::unexpected_constraint<E>, "not a valid type for expected error type");
  static_assert(concepts::expected_constraint<T>, "not a valid type for expected value type");
  using value_type = T;
  using error_type = E;
  using unexpected_type = unexpected<E>;
  template<typename U>
  using rebind = expected<U, error_type>;
  friend struct detail::swap_expected_t;

private:
  using bad_access_exception = bad_expected_access<std::decay_t<error_type>>;

  static constexpr bool value_copy_constructible = std::is_copy_constructible_v<value_type>;
  static constexpr bool value_move_constructible = std::is_move_constructible_v<value_type>;
  static constexpr bool value_copy_assignable = std::is_copy_assignable_v<value_type>;
  static constexpr bool value_move_assignable = std::is_move_assignable_v<value_type>;

  static constexpr bool value_nothrow_copy_constructible = std::is_nothrow_copy_constructible_v<value_type>;
  static constexpr bool value_nothrow_copy_assignable = std::is_nothrow_copy_assignable_v<value_type>;
  static constexpr bool value_nothrow_move_constructible = std::is_nothrow_move_constructible_v<value_type>;
  static constexpr bool value_nothrow_move_assignable = std::is_nothrow_move_assignable_v<value_type>;

  static constexpr bool error_copy_constructible = std::is_copy_constructible_v<error_type>;
  static constexpr bool error_copy_assignable = std::is_copy_assignable_v<error_type>;
  static constexpr bool error_move_constructible = std::is_move_constructible_v<error_type>;
  static constexpr bool error_move_assignable = std::is_move_assignable_v<error_type>;

  static constexpr bool error_nothrow_copy_assignable = std::is_nothrow_copy_assignable_v<error_type>;
  static constexpr bool error_nothrow_copy_constructible = std::is_nothrow_copy_constructible_v<error_type>;

  static constexpr bool error_nothrow_move_constructible = std::is_nothrow_move_constructible_v<error_type>;
  static constexpr bool error_nothrow_move_assignable = std::is_nothrow_move_assignable_v<error_type>;

  static constexpr bool value_trivially_copy_constructible = std::is_trivially_copy_constructible_v<value_type>;

  static constexpr bool error_trivially_copy_constructible = std::is_trivially_copy_constructible_v<error_type>;

  static constexpr bool both_are_nothrow_move_constructible
    = value_nothrow_move_constructible && error_nothrow_move_constructible;

  static constexpr bool both_are_trivially_copy_constructible
    = value_trivially_copy_constructible && error_trivially_copy_constructible;

  static constexpr bool both_are_trivially_move_constructible
    = std::is_trivially_move_constructible_v<value_type> && std::is_trivially_move_constructible_v<error_type>;

    union {
    T value_;
    E error_;
    };

  bool has_value_;

public:
  constexpr expected() noexcept(std::is_nothrow_default_constructible_v<value_type>)
    requires std::is_default_constructible_v<value_type>
      : has_value_{true}
    {
    std::construct_at(std::addressof(value_));
    }

  constexpr expected(expected const &) noexcept
    requires value_trivially_copy_constructible && error_trivially_copy_constructible
  = default;

  constexpr expected(expected const & rh) noexcept(value_nothrow_move_constructible && error_nothrow_move_constructible)
    requires(value_copy_constructible) && error_copy_constructible && (!both_are_trivially_copy_constructible)
      : has_value_(rh.has_value_)
    {
    if(has_value_) [[likely]]
      std::construct_at(std::addressof(value_), rh.value_);
    else
      std::construct_at(std::addressof(error_), rh.error_);
    }

  constexpr expected(expected &&) noexcept
    requires both_are_trivially_move_constructible
  = default;

  constexpr expected(expected && rh) noexcept(both_are_nothrow_move_constructible)
    requires value_move_constructible && error_move_constructible && (!both_are_trivially_move_constructible)
      : has_value_(rh.has_value_)
    {
    if(has_value_) [[likely]]
      std::construct_at(std::addressof(value_), std::move(rh.value_));
    else
      std::construct_at(std::addressof(error_), std::move(rh.error_));
    }

  template<typename U, typename G>
    requires requires {
      requires std::is_constructible_v<T, std::add_lvalue_reference_t<U const>>;
      requires std::is_constructible_v<E, G const &>;
      requires concepts::expected_conv_constr<T, E, U, G>;
    }
  constexpr explicit(!std::is_convertible_v<std::add_lvalue_reference_t<U const>, T> || !std::is_convertible_v<G const &, E>) expected(expected<U, G> const & rh) noexcept(
    std::
      is_nothrow_constructible_v<value_type, decltype(std::forward<std::add_lvalue_reference_t<U const>>(rh.value()))>
    && std::is_nothrow_constructible_v<error_type, decltype(std::forward<G const &>(rh.error()))>
  ) :
      has_value_{rh.has_value()}
    {
    if(has_value_) [[likely]]
      std::construct_at(std::addressof(value_), std::forward<std::add_lvalue_reference_t<U const>>(rh.value()));
    else
      std::construct_at(std::addressof(error_), std::forward<G const &>(rh.error()));
    }

  template<typename U, typename G>
    requires requires {
      requires std::is_constructible_v<T, U>;
      requires std::is_constructible_v<E, G>;
      requires concepts::expected_conv_constr<T, E, U, G>;
    }
  constexpr explicit(!std::is_convertible_v<U, T> || !std::is_convertible_v<G, E>) expected(expected<U, G> && rh) noexcept(
    std::is_nothrow_constructible_v<value_type, decltype(std::forward<U>(rh.value()))>
    && std::is_nothrow_constructible_v<error_type, decltype(std::forward<G>(rh.error()))>
  ) :
      has_value_{rh.has_value()}
    {
    if(has_value_) [[likely]]
      std::construct_at(std::addressof(value_), std::forward<U>(rh.value()));
    else
      std::construct_at(std::addressof(error_), std::forward<G>(rh.error()));
    }

  template<typename U = T>
    requires requires {
      requires !std::same_as<std::remove_cvref_t<U>, std::in_place_t>;
      requires !std::same_as<expected, std::remove_cvref_t<U>>;
      requires std::is_constructible_v<T, U>;
      requires !concepts::is_unexpected<std::remove_cvref_t<U>>;
      requires !std::same_as<bool, T> || !concepts::is_expected<std::remove_cvref_t<U>>;
    }
  constexpr explicit(!std::is_convertible_v<U, T>
  ) expected(U && v) noexcept(std::is_nothrow_constructible_v<value_type, decltype(std::forward<U>(v))>) :
      has_value_{true}
    {
    std::construct_at(std::addressof(value_), std::forward<U>(v));
    }

  template<typename... Args>
    requires std::constructible_from<value_type, Args...>
  constexpr explicit expected(std::in_place_t, Args &&... args) noexcept(noexcept(std::forward<Args...>(args...))) :
      has_value_{true}
    {
    std::construct_at(std::addressof(value_), std::forward<Args>(args)...);
    }

  template<typename U, typename... Args>
    requires std::constructible_from<value_type, std::initializer_list<U> &, Args...>
  constexpr explicit expected(std::in_place_t, std::initializer_list<U> il, Args &&... args) : has_value_{true}
    {
    std::construct_at(std::addressof(value_), il, std::forward<Args>(args)...);
    }

  template<typename... Args>
    requires std::constructible_from<error_type, Args...>
  constexpr explicit expected(unexpect_t, Args &&... args) noexcept(noexcept(std::forward<Args...>(args...))) :
      has_value_{}
    {
    std::construct_at(std::addressof(error_), std::forward<Args>(args)...);
    }

  template<typename U, typename... Args>
    requires std::constructible_from<error_type, std::initializer_list<U> &, Args...>
  constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args &&... args) : has_value_{}
    {
    std::construct_at(std::addressof(error_), il, std::forward<Args>(args)...);
    }

  template<typename G>
    requires std::is_constructible_v<E, G const &>
  constexpr explicit(!std::is_convertible_v<G const &, error_type>) expected(unexpected<G> const & e) : has_value_{}
    {
    std::construct_at(std::addressof(error_), e.error());
    }

  template<typename G>
    requires std::is_constructible_v<E, G>
  constexpr explicit(!std::is_convertible_v<G, E>) expected(unexpected<G> && e) : has_value_{}
    {
    std::construct_at(std::addressof(error_), std::forward<G>(e.error()));
    }

  constexpr ~expected()
    requires std::is_trivially_destructible_v<value_type> && std::is_trivially_destructible_v<error_type>
  = default;

  constexpr ~expected()
    requires(!std::is_trivially_destructible_v<value_type> || !std::is_trivially_destructible_v<error_type>)
    {
    if(has_value_) [[likely]]
      {
      if constexpr(!std::is_trivially_destructible_v<value_type>)
        std::destroy_at(std::addressof(value_));
      }
    else
      {
      if constexpr(!std::is_trivially_destructible_v<error_type>)
        std::destroy_at(std::addressof(error_));
      }
    }

private:
  template<typename ET, typename EU, typename A>
  static constexpr void
    reinit_expected(ET * new_value, EU * old_value, A && arg) noexcept(std::is_nothrow_constructible_v<ET, A>)
    {
    if constexpr(std::is_nothrow_constructible_v<ET, A>)
      {
      std::destroy_at(old_value);
      std::construct_at(new_value, std::forward<A>(arg));
      }
    else if constexpr(std::is_nothrow_move_constructible_v<ET>)
      {
      ET tmp(std::forward<A>(arg));
      std::destroy_at(old_value);
      std::construct_at(new_value, std::move(tmp));
      }
    else
      {
      static_assert(std::is_nothrow_move_constructible_v<EU>);
      detail::revert_if_except_t<false, EU> obj(std::move(*old_value), old_value);
      std::construct_at(new_value, std::forward<A>(arg));
      obj.release();
      }
    }

  template<typename other_value_type>
  inline constexpr void
    assign_value(other_value_type && v) noexcept(std::is_nothrow_constructible_v<value_type, other_value_type>)
    {
    if(has_value_)
      value_ = std::forward<other_value_type>(v);
    else
      {
      reinit_expected(std::addressof(value_), std::addressof(error_), std::forward<other_value_type>(v));
      has_value_ = true;
      }
    }

  template<typename other_error_type>
  inline constexpr void
    assign_unexpected(other_error_type && v) noexcept(std::is_nothrow_constructible_v<error_type, other_error_type>)
    {
    if(has_value_)
      {
      reinit_expected(std::addressof(error_), std::addressof(value_), std::forward<other_error_type>(v));
      has_value_ = false;
      }
    else
      error_ = std::forward<other_error_type>(v);
    }

public:
  constexpr auto operator=(expected const &) -> expected & = delete;

  constexpr expected & operator=(expected const & rh) noexcept(
    value_nothrow_copy_assignable and value_nothrow_copy_constructible and error_nothrow_copy_assignable
    and error_nothrow_copy_constructible
  )
    requires(
      value_copy_assignable and value_copy_constructible and error_copy_assignable and error_copy_constructible
      and (value_nothrow_move_constructible or error_nothrow_move_constructible)
    )
    {
    if(has_value_ and rh.has_value())
      value_ = rh.value_;
    else if(has_value_)
      assign_unexpected(rh.error_);
    else if(rh.has_value())
      assign_value(rh.value_);
    else
      error_ = rh.error_;
    return *this;
    }

  constexpr expected & operator=(expected && rh) noexcept(
    value_nothrow_move_assignable and value_nothrow_move_constructible and error_nothrow_move_assignable
    and error_nothrow_move_constructible
  )
    requires(
      value_move_constructible and value_move_assignable and error_move_constructible and error_move_assignable
      and (value_nothrow_move_constructible or error_nothrow_move_constructible)
    )
    {
    if(has_value_ && rh.has_value())
      value_ = std::move(rh.value_);
    else if(has_value_)
      assign_unexpected(std::move(rh.error_));
    else if(rh.has_value())
      assign_value(std::move(rh.value_));
    else
      error_ = std::move(rh.error_);
    return *this;
    }

  template<typename Up = value_type>
  constexpr auto operator=(Up && v) -> expected &
    requires(
      not std::is_same_v<expected, std::remove_cvref_t<Up>> && not concepts::is_unexpected<std::remove_cvref_t<Up>>
      and std::is_constructible_v<value_type, Up> && std::is_assignable_v<value_type &, Up>
      and (std::is_nothrow_constructible_v<value_type, Up> or value_nothrow_move_constructible or error_nothrow_move_constructible)
    )
    {
    if(has_value_)
      value_ = std::forward<Up>(v);
    else
      assign_value(std::forward<Up>(v));
    return *this;
    }

  template<typename other_error>
    requires std::is_constructible_v<error_type, other_error const &>
             and std::is_assignable_v<error_type &, other_error const &>
             && (std::is_nothrow_constructible_v<error_type, other_error const &> || value_nothrow_move_constructible || error_nothrow_move_constructible)
  constexpr auto operator=(unexpected<other_error> const & e) -> expected &
    {
    assign_unexpected(e.error());
    return *this;
    }

  template<typename other_error>
    requires std::is_constructible_v<error_type, other_error> && std::is_assignable_v<error_type &, other_error>
             && (std::is_nothrow_constructible_v<error_type, other_error> || value_nothrow_move_constructible || error_nothrow_move_constructible)
  constexpr auto operator=(unexpected<other_error> && e) -> expected &
    {
    assign_unexpected(std::move(e).error());
    return *this;
    }

public:
  [[nodiscard]]
  constexpr auto operator->() const noexcept -> value_type const *
    {
    assert(has_value_);
    return std::addressof(value_);
    }

  [[nodiscard]]
  constexpr auto operator->() noexcept -> value_type *
    {
    assert(has_value_);
    return std::addressof(value_);
    }

  [[nodiscard]]
  constexpr auto operator*() const & noexcept -> value_type const &
    {
    assert(has_value_);
    return value_;
    }

  [[nodiscard]]
  constexpr auto operator*() & noexcept -> value_type &
    {
    assert(has_value_);
    return value_;
    }

  [[nodiscard]]
  constexpr auto operator*() const && noexcept -> value_type const &&
    {
    assert(has_value_);
    return std::move(value_);
    }

  [[nodiscard]]
  constexpr auto operator*() && noexcept -> value_type &&
    {
    assert(has_value_);
    return std::move(value_);
    }

  [[nodiscard]]
  constexpr explicit operator bool() const noexcept
    {
    return has_value_;
    }

  [[nodiscard]]
  constexpr bool has_value() const noexcept
    {
    return has_value_;
    }

  [[nodiscard]]
  constexpr auto value() & -> value_type &
    requires error_copy_constructible
    {
    if(has_value_) [[likely]]
      return value_;
    else
      throw bad_access_exception{error_};
    }

  [[nodiscard]]
  constexpr auto value() const & -> value_type const &
    requires error_copy_constructible
    {
    if(has_value_) [[likely]]
      return value_;
    else
      throw bad_access_exception{error_};
    }

  [[nodiscard]]
  constexpr auto value() && -> value_type && requires(error_copy_constructible || error_move_constructible) {
    if(has_value_) [[likely]]
      return std::move(value_);
    else
      throw bad_access_exception{std::move(error_)};
  }

  [[nodiscard]] constexpr auto value() const && -> value_type const &&
    requires(error_copy_constructible || error_move_constructible)
    {
    if(has_value_) [[likely]]
      return std::move(value_);
    else
      throw bad_access_exception{std::move(error_)};
    }

  [[nodiscard]]
  constexpr auto error() const & noexcept -> error_type const &
    {
    assert(!has_value_);
    return error_;
    }

  [[nodiscard]]
  constexpr auto error() & noexcept -> error_type &
    {
    assert(!has_value_);
    return error_;
    }

  [[nodiscard]]
  constexpr auto error() const && noexcept -> error_type const &&
    {
    assert(!has_value_);
    return std::move(error_);
    }

  [[nodiscard]]
  constexpr auto error() && noexcept -> error_type &&
    {
    assert(!has_value_);
    return std::move(error_);
    }

  template<typename U>
  [[nodiscard]]
  constexpr auto value_or(U && default_value) const & noexcept(
    std::is_nothrow_copy_constructible_v<value_type> && std::is_nothrow_convertible_v<U, value_type>
  ) -> value_type
    requires value_copy_constructible && std::is_convertible_v<U, value_type>
    {
    return has_value_ ? value_ : static_cast<value_type>(std::forward<U>(default_value));
    }

  template<typename U>
  [[nodiscard]]
  constexpr auto value_or(U && default_value) && noexcept(
    std::is_nothrow_move_constructible_v<value_type> && std::is_nothrow_convertible_v<U, value_type>
  ) -> value_type
    requires value_move_constructible && std::is_convertible_v<U, value_type>
    {
    return has_value_ ? std::move(value_) : static_cast<value_type>(std::forward<U>(default_value));
    }

  template<typename F>
  constexpr auto and_then(F && f) &
    requires error_copy_constructible
    {
    return detail::and_then(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto and_then(F && f) const &
    requires error_copy_constructible
    {
    return detail::and_then(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto and_then(F && f) &&
    requires error_move_constructible
    {
    return detail::and_then(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto and_then(F && f) const &&
    requires error_move_constructible
    {
    return detail::and_then(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform(F && f) &
    requires error_copy_constructible
    {
    return detail::transform(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform(F && f) const &
    requires error_copy_constructible
    {
    return detail::transform(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform(F && f) &&
    requires error_move_constructible
    {
    return detail::transform(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform(F && f) const &&
    requires error_move_constructible
    {
    return detail::transform(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto or_else(F && f) &
    requires value_copy_constructible
    {
    return detail::or_else(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto or_else(F && f) const &
    requires value_copy_constructible
    {
    return detail::or_else(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto or_else(F && f) &&
    requires value_move_constructible
    {
    return detail::or_else(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto or_else(F && f) const &&
    requires value_move_constructible
    {
    return detail::or_else(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform_error(F && f) &
    {
    return detail::transform_error(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform_error(F && f) const &
    {
    return detail::transform_error(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform_error(F && f) &&
    {
    return detail::transform_error(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform_error(F && f) const &&
    {
    return detail::transform_error(std::move(*this), std::forward<F>(f));
    }

  template<typename... Args>
  constexpr value_type & emplace(Args &&... args) noexcept
    requires std::is_nothrow_constructible_v<value_type, Args...>
    {
    if(has_value_) [[likely]]
      {
      if constexpr(!std::is_trivially_destructible_v<value_type>)
        std::destroy_at(std::addressof(value_));
      }
    else
      {
      if constexpr(!std::is_trivially_destructible_v<error_type>)
        std::destroy_at(std::addressof(error_));
      has_value_ = true;
      }
    return *std::construct_at(std::addressof(value_), std::forward<Args>(args)...);
    }

  template<typename U, typename... Args>
  constexpr value_type & emplace(std::initializer_list<U> & il, Args &&... args) noexcept
    requires std::is_nothrow_constructible_v<value_type, std::initializer_list<U> &, Args...>
    {
    if(has_value_) [[likely]]
      {
      if constexpr(!std::is_trivially_destructible_v<value_type>)
        std::destroy_at(std::addressof(value_));
      }
    else
      {
      if constexpr(!std::is_trivially_destructible_v<error_type>)
        std::destroy_at(std::addressof(error_));
      has_value_ = true;
      }
    return *std::construct_at(std::addressof(value_), il, std::forward<Args>(args)...);
    }

  constexpr void swap(expected & other) noexcept(detail::swap_no_throw<T, E>)
    requires concepts::swap_constraints<T, E>
    {
    detail::swap_dispatch(*this, other);
    }

  template<typename T2, typename E2>
    requires requires {
      requires(!std::is_void_v<T2>);
      requires std::equality_comparable_with<value_type, T2>;
      requires std::equality_comparable_with<error_type, E2>;
    }
  friend constexpr bool operator==(expected const & lhs, expected<T2, E2> const & rhs) noexcept(
    noexcept(lhs.value() == rhs.value()) && noexcept(lhs.error() == rhs.error())
  )
    {
    if(lhs.has_value() == rhs.has_value())
      if(lhs.has_value())
        return lhs.value() == rhs.value();
      else
        return lhs.error() == rhs.error();
    else
      return false;
    }

  template<typename T2>
    requires requires {
      requires concepts::not_expected<T2>;
      requires std::equality_comparable_with<value_type, T2>;
    }
  friend constexpr bool operator==(expected const & x, T2 const & val) noexcept(noexcept(x.value() == val))
    {
    return x.has_value() && (x.value() == val);
    }

  template<std::equality_comparable_with<error_type> E2>
  friend constexpr bool
    operator==(expected const & x, unexpected<E2> const & e) noexcept(noexcept(x.error() == e.error()))
    {
    if(!x.has_value())
      return x.error() == e.error();
    return false;
    }

  friend constexpr void swap(expected & lhs, expected & rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }
  };

template<typename T, typename E>
  requires std::same_as<void, T>
class [[nodiscard, clang::trivial_abi]] expected<T, E>
  {
public:
  using value_type = void;
  using error_type = E;
  using unexpected_type = unexpected<E>;
  template<typename U>
  using rebind = expected<U, error_type>;
  friend struct detail::swap_expected_t;

private:
  using bad_access_exception = bad_expected_access<std::decay_t<error_type>>;
  static constexpr bool error_nothrow_copy_constructible = std::is_nothrow_copy_constructible_v<error_type>;
  static constexpr bool error_trivially_copy_constructible = std::is_trivially_copy_constructible_v<error_type>;
  static constexpr bool error_nothrow_move_constructible = std::is_nothrow_move_constructible_v<error_type>;
  static constexpr bool error_nothrow_move_assignable = std::is_nothrow_move_assignable_v<error_type>;
  static constexpr bool error_trivially_move_constructible = std::is_trivially_move_constructible_v<error_type>;
  static constexpr bool error_copy_constructible = std::is_copy_constructible_v<error_type>;
  static constexpr bool error_move_constructible = std::is_move_constructible_v<error_type>;

  static constexpr bool error_nothrow_copy_assignable = std::is_nothrow_copy_assignable_v<error_type>;
  static constexpr bool error_copy_assignable = std::is_copy_assignable_v<error_type>;
  static constexpr bool error_move_assignable = std::is_move_assignable_v<error_type>;

    union {
    E error_;
    };

  bool has_value_;

public:
  constexpr expected() noexcept : has_value_{true} {}

  constexpr expected(expected const &) noexcept
    requires error_trivially_copy_constructible
  = default;

  constexpr expected(expected const & rh) noexcept(error_nothrow_move_constructible)
    requires error_copy_constructible && (!error_trivially_copy_constructible)
      : has_value_(rh.has_value_)
    {
    if(!has_value_) [[likely]]
      std::construct_at(std::addressof(error_), rh.error_);
    }

  constexpr expected(expected &&) noexcept
    requires error_trivially_move_constructible
  = default;

  constexpr expected(expected && rh) noexcept(error_nothrow_move_constructible)
    requires error_move_constructible && (!error_trivially_move_constructible)
      : has_value_(rh.has_value_)
    {
    if(!has_value_) [[unlikely]]
      std::construct_at(std::addressof(error_), std::move(rh.error_));
    }

  template<typename U, typename G>
    requires requires {
      requires std::same_as<U, void>;
      requires std::is_constructible_v<E, G const &>;
      requires concepts::expected_conv_constr<void, E, U, G>;
    }
  constexpr explicit(!std::is_convertible_v<G const &, E>) expected(
    expected<U, G> const & rh
  ) noexcept(std::is_nothrow_constructible_v<error_type, decltype(std::forward<G const &>(rh.error()))>) :
      has_value_{rh.has_value()}
    {
    if(!has_value_) [[unlikely]]
      std::construct_at(std::addressof(error_), std::forward<G const &>(rh.error()));
    }

  template<typename U, typename G>
    requires requires {
      requires std::same_as<U, void>;
      requires std::is_constructible_v<E, G>;
      requires concepts::expected_conv_constr<void, E, U, G>;
    }
  constexpr explicit(!std::is_convertible_v<G, E>) expected(
    expected<U, G> && rh
  ) noexcept(std::is_nothrow_constructible_v<error_type, decltype(std::forward<G>(rh.error()))>) :
      has_value_{rh.has_value()}
    {
    if(!has_value_) [[unlikely]]
      std::construct_at(std::addressof(error_), std::forward<G>(rh.error()));
    }

  template<class... Args>
  constexpr explicit expected(std::in_place_t) noexcept : has_value_{true}
    {
    }

  template<typename... Args>
    requires std::constructible_from<error_type, Args...>
  constexpr explicit expected(unexpect_t, Args &&... args) noexcept(noexcept(std::forward<Args...>(args...))) :
      has_value_{}
    {
    std::construct_at(std::addressof(error_), std::forward<Args>(args)...);
    }

  template<typename U, typename... Args>
    requires std::constructible_from<error_type, std::initializer_list<U> &, Args...>
  constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args &&... args) : has_value_{}
    {
    std::construct_at(std::addressof(error_), il, std::forward<Args>(args)...);
    }

  template<typename G>
    requires std::is_constructible_v<E, G const &>
  inline constexpr explicit(!std::is_convertible_v<G const &, error_type>) expected(unexpected<G> const & e) :
      has_value_{}
    {
    std::construct_at(std::addressof(error_), e.error());
    }

  template<typename G>
    requires std::is_constructible_v<E, G>
  constexpr explicit(!std::is_convertible_v<G, E>) expected(unexpected<G> && e) : has_value_{}
    {
    std::construct_at(std::addressof(error_), std::forward<G>(e.error()));
    }

private:
  template<typename other_error_type>
  inline constexpr void assign_unexpected(other_error_type && v)
    {
    if(has_value_)
      {
      std::construct_at(std::addressof(error_), std::forward<other_error_type>(v));
      has_value_ = false;
      }
    else
      error_ = std::forward<other_error_type>(v);
    }

public:
  auto operator=(expected const &) -> expected & = delete;

  constexpr auto operator=(expected const & rh
  ) noexcept(error_nothrow_copy_constructible and error_nothrow_copy_assignable) -> expected &
    requires error_copy_constructible and error_copy_assignable
    {
    if(rh.has_value_)
      emplace();
    else
      assign_unexpected(rh.error_);
    return *this;
    }

  constexpr auto operator=(expected && rh) noexcept(error_nothrow_move_constructible and error_nothrow_move_assignable)
    -> expected &
    requires error_move_constructible && error_move_assignable
    {
    if(rh.has_value_)
      emplace();
    else
      assign_unexpected(std::move(rh.error_));
    return *this;
    }

  template<typename G>
    requires std::is_constructible_v<error_type, G const &> && std::is_assignable_v<error_type &, G const &>
  constexpr auto operator=(unexpected<G> const & ux) -> expected &
    {
    assign_unexpected(ux.error());
    return *this;
    }

  template<typename G>
    requires std::is_constructible_v<error_type, G> && std::is_assignable_v<error_type &, G>
  constexpr auto operator=(unexpected<G> && ux) -> expected &
    {
    assign_unexpected(std::move(ux).error());
    return *this;
    }

  constexpr ~expected()
    requires std::is_trivially_destructible_v<error_type>
  = default;

  constexpr ~expected()
    requires(!std::is_trivially_destructible_v<error_type>)
    {
    if(!has_value_) [[unlikely]]
      std::destroy_at(std::addressof(error_));
    }

  constexpr void operator*() const noexcept { assert(has_value_); }

  [[nodiscard]]
  constexpr explicit operator bool() const noexcept
    {
    return has_value_;
    }

  [[nodiscard]]
  constexpr bool has_value() const noexcept
    {
    return has_value_;
    }

  constexpr void value() const &
    requires error_copy_constructible
    {
    if(!has_value_) [[unlikely]]
      throw bad_access_exception{error_};
    }

  constexpr void value() &&
    requires(error_copy_constructible || error_move_constructible)
    {
    if(!has_value_) [[unlikely]]
      throw bad_access_exception{std::move(error_)};
    }

  [[nodiscard]]
  constexpr auto error() const & noexcept -> error_type const &
    {
    assert(!has_value_);
    return error_;
    }

  [[nodiscard]]
  constexpr auto error() & noexcept -> error_type &
    {
    assert(!has_value_);
    return error_;
    }

  [[nodiscard]]
  constexpr auto error() const && noexcept -> error_type const &&
    {
    assert(!has_value_);
    return std::move(error_);
    }

  [[nodiscard]]
  constexpr auto error() && noexcept -> error_type &&
    {
    assert(!has_value_);
    return std::move(error_);
    }

  template<typename F>
  constexpr auto and_then(F && f) &
    requires error_copy_constructible
    {
    return detail::and_then(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto and_then(F && f) const &
    requires error_copy_constructible
    {
    return detail::and_then(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto and_then(F && f) &&
    requires error_move_constructible
    {
    return detail::and_then(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto and_then(F && f) const &&
    requires error_move_constructible
    {
    return detail::and_then(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform(F && f) &
    requires error_copy_constructible
    {
    return detail::transform(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform(F && f) const &
    requires error_copy_constructible
    {
    return detail::transform(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform(F && f) &&
    requires error_move_constructible
    {
    return detail::transform(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform(F && f) const &&
    requires error_move_constructible
    {
    return detail::transform(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto or_else(F && f) &
    {
    return detail::or_else(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto or_else(F && f) const &
    {
    return detail::or_else(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto or_else(F && f) &&
    {
    return detail::or_else(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto or_else(F && f) const &&
    {
    return detail::or_else(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform_error(F && f) &
    {
    return detail::transform_error(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform_error(F && f) const &
    {
    return detail::transform_error(*this, std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform_error(F && f) &&
    {
    return detail::transform_error(std::move(*this), std::forward<F>(f));
    }

  template<typename F>
  constexpr auto transform_error(F && f) const &&
    {
    return detail::transform_error(std::move(*this), std::forward<F>(f));
    }

  constexpr void emplace() noexcept
    {
    if(!has_value_) [[likely]]
      {
      if constexpr(!std::is_trivially_destructible_v<error_type>)
        std::destroy_at(std::addressof(error_));
      has_value_ = true;
      }
    }

  constexpr void swap(expected & other) noexcept(detail::swap_no_throw<void, E>)
    requires concepts::swap_constraints<void, E>
    {
    detail::swap_dispatch(*this, other);
    }

  template<typename T2, typename E2>
    requires requires {
      requires std::is_void_v<T2>;
      requires std::equality_comparable_with<error_type, E2>;
    }
  friend constexpr bool
    operator==(expected const & lhs, expected<T2, E2> const & rhs) noexcept(noexcept(lhs.error() == rhs.error()))
    {
    if(lhs.has_value() == rhs.has_value())
      return lhs.has_value() || lhs.error() == rhs.error();
    else
      return false;
    }

  template<std::equality_comparable_with<error_type> E2>
  friend constexpr bool operator==(expected const & x, unexpected<E2> const & e)
    {
    if(!x.has_value())
      return x.error() == e.error();
    return false;
    }

  friend constexpr void swap(expected & lhs, expected & rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }
  };

namespace detail
  {
  template<typename F, typename T>
  struct and_then_invoke_result
    {
    using type = std::remove_cvref_t<std::invoke_result_t<F, decltype(std::declval<T>())>>;
    };

  template<typename F>
  struct and_then_invoke_result<F, void>
    {
    using type = std::remove_cvref_t<std::invoke_result_t<F>>;
    };

  template<typename F, typename T>
  using and_then_invoke_result_t = typename and_then_invoke_result<F, T>::type;

  template<typename EX, typename F>
  constexpr auto and_then(EX && ex, F && f)
    {
    using expected_type = std::remove_cvref_t<EX>;
    using U = and_then_invoke_result_t<F, decltype(std::forward<EX>(ex).value())>;
    if(ex.has_value())
      if constexpr(std::is_void_v<typename expected_type::value_type>)
        return std::invoke(std::forward<F>(f));
      else
        return std::invoke(std::forward<F>(f), std::forward<EX>(ex).value());
    else
      return U(unexpect, std::forward<EX>(ex).error());
    }

  template<typename F, typename T>
  struct transform_invoke_result
    {
    using type = std::remove_cv_t<std::invoke_result_t<F, decltype(std::declval<T>())>>;
    };

  template<typename F>
  struct transform_invoke_result<F, void>
    {
    using type = std::remove_cv_t<std::invoke_result_t<F>>;
    };

  template<typename F, typename T>
  using transform_invoke_result_t = typename transform_invoke_result<F, T>::type;

  template<typename EX, typename F>
  constexpr auto transform(EX && ex, F && f)
    {
    using expected_type = std::remove_cvref_t<EX>;
    using U = transform_invoke_result_t<F, decltype(std::forward<EX>(ex).value())>;

    using error_type = typename expected_type::error_type;
    if(ex.has_value())
      if constexpr(std::is_void_v<typename expected_type::value_type>)
        {
        std::invoke(std::forward<F>(f));
        return expected<U, error_type>{std::in_place};
        }
      else
        return expected<U, error_type>{std::in_place, std::invoke(std::forward<F>(f), std::forward<EX>(ex).value())};
    else
      return expected<U, error_type>(unexpect, std::forward<EX>(ex).error());
    }

  template<typename EX, typename F>
  constexpr auto or_else(EX && ex, F && f)
    {
    using G = std::remove_cvref_t<std::invoke_result_t<F, decltype(std::forward<EX>(ex).error())>>;
    static_assert(std::is_same_v<typename G::value_type, typename EX::value_type>);
    if(ex.has_value())
      if constexpr(std::is_void_v<typename EX::value_type>)
        return G();
      else
        return G(std::in_place, std::forward<EX>(ex).value());
    else
      return std::invoke(std::forward<F>(f), std::forward<EX>(ex).error());
    }

  template<typename EX, typename F>
  constexpr auto transform_error(EX && ex, F && f)
    {
    using expected_type = std::remove_cvref_t<EX>;
    using G = std::remove_cv_t<std::invoke_result_t<F, decltype(std::forward<EX>(ex).error())>>;
    using value_type = typename expected_type::value_type;
    if(ex.has_value())
      if constexpr(std::is_void_v<typename expected_type::value_type>)
        return expected<value_type, G>();
      else
        return expected<value_type, G>(std::in_place, std::forward<EX>(ex).value());
    else
      {
      auto invoke_res{std::invoke(std::forward<F>(f), std::forward<EX>(ex).error())};
      return expected<value_type, G>(unexpect, invoke_res);
      }
    }

  struct swap_expected_t
    {
    template<typename T, typename E>
    static constexpr void operator()(expected<T, E> & l, expected<T, E> & r) noexcept(detail::swap_no_throw<T, E>)
      requires concepts::swap_constraints<T, E>
      {
      if(l.has_value() && r.has_value())
        {
        if constexpr(!std::is_void_v<T>)
          std::swap(l.value(), r.value());
        }
      else if(!l.has_value() && !r.has_value())
        {
        std::swap(l.error(), r.error());
        }
      else if(!l.has_value() && r.has_value())
        operator()(r, l);
      else
        {
        auto const r_addr_error{std::addressof(r.error_)};
        auto const l_addr_error{std::addressof(l.error_)};
        if constexpr(std::is_void_v<T>)
          {
          std::construct_at(l_addr_error, std::move(r.error()));
          std::destroy_at(r_addr_error);
          std::swap(l.has_value_, r.has_value_);
          }
        else
          {
          auto const r_addr_val{std::addressof(r.value_)};
          auto const l_addr_val{std::addressof(l.value_)};
          if constexpr(std::is_nothrow_move_constructible_v<E>)
            {
            using revert = revert_if_except_t<std::is_nothrow_move_constructible_v<T>, E>;
            revert temp{std::move(r.error()), r_addr_error};
            std::destroy_at(r_addr_error);
            std::construct_at(r_addr_val, std::move(l.value()));
            std::destroy_at(l_addr_val);
            std::construct_at(l_addr_error, temp.release());
            std::swap(l.has_value_, r.has_value_);
            }
          else
            {
            using revert = revert_if_except_t<false, T>;
            revert temp{std::move(l.value()), l_addr_val};
            std::destroy_at(l_addr_val);
            std::construct_at(l_addr_error, std::move(r.error()));
            std::destroy_at(r_addr_error);
            std::construct_at(r_addr_val, temp.release());
            std::swap(l.has_value_, r.has_value_);
            }
          }
        }
      }
    };

  template<concepts::is_expected EX>
  inline constexpr void swap_dispatch(EX & l, EX & r)
    {
    swap_expected_t{}(l, r);
    }
  }  // namespace detail

  }  // namespace cxx23
#endif
#endif
