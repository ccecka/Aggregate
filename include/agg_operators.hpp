#pragma once

#include <iostream>
#include <cassert>

namespace agg {

//! Granulated sugar
template <typename Bool, typename Type>
using enable_if = typename std::enable_if<Bool::value, Type>::type;

template <typename Type>
using decay_t = typename std::decay<Type>::type;


namespace detail {

//! C++14-style integer_sequences for compile-time unrolling
template <class T, T... Ints>
struct integer_sequence {};
template <std::size_t... Ints>
using index_sequence = integer_sequence<std::size_t, Ints...>;

template <class T, std::size_t N, T... Is>
struct generate_integer_sequence {
  using type = typename generate_integer_sequence<T, N-1, N-1, Is...>::type;
};
template <class T, T... Is>
struct generate_integer_sequence<T, 0, Is...> {
  using type = integer_sequence<T, Is...>;
};

template <class T, T N>
using make_integer_sequence = typename generate_integer_sequence<T, N>::type;
template <std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;


template <std::size_t I, typename Fn, typename... Tuples>
auto tuple_map_invoke(Fn f, Tuples&&... ts)
    -> decltype(f(std::get<I>(std::forward<Tuples>(ts))...)) {
  return f(std::get<I>(std::forward<Tuples>(ts))...);
}

template <typename R>
struct tuple_map_make_impl {
  template <std::size_t... I, typename Fn, typename... Tuples>
  static R apply(index_sequence<I...>, Fn f, Tuples&&... ts) {
    return {tuple_map_invoke<I>(f, std::forward<Tuples>(ts)...)...};
  }
};

template <>
struct tuple_map_make_impl<void> {
  template <std::size_t... I, typename Fn, typename... Tuples>
  static void apply(index_sequence<I...>, Fn f, Tuples&&... ts) {
    auto l =
        { (tuple_map_invoke<I>(f, std::forward<Tuples>(ts)...), void(), 0)... };
    (void) l;
  }
};

// TODO: Default or deduce the return type R?
template <typename R, typename Fn, typename Tuple, typename... Tuples>
R tuple_map(Fn f, Tuple&& t, Tuples&&... ts) {
  return tuple_map_make_impl<R>::apply(
      make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>(),
      f,
      t,
      std::forward<Tuples>(ts)...);
}

} // end namespace detail


// Aggregate operations

//! Write to an output stream
template <typename CharT, typename Traits, typename T, std::size_t N>
inline
enable_if<
  has_left_shift<std::basic_ostream<CharT,Traits>&,T>,
  std::ostream&>
operator<<(std::basic_ostream<CharT,Traits>& s, const aggregate<T,N>& a) {
  std::copy(a.begin(), a.end()-1, std::ostream_iterator<T>(s, " "));
  if (N >= 1) s << a[N-1];
  return s;
}


// Aggregate comparisons.
template <typename T, typename U, std::size_t N>
inline
enable_if<
  has_equal_to<T,U,bool>,
  bool>
operator==(const aggregate<T,N>& a, const aggregate<U,N>& b) {
  return std::equal(a.begin(), a.end(), b.begin());
}

template <typename T, typename U, std::size_t N>
inline
enable_if<
  has_not_equal_to<T,U,bool>,
  bool>
operator!=(const aggregate<T,N>& a, const aggregate<U,N>& b) {
  return !(a == b);
}

template <typename T, typename U, std::size_t N>
inline
enable_if<
  has_less<T,U,bool>,
  bool>
operator<(const aggregate<T,N>& a, const aggregate<U,N>& b) {
  return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template <typename T, typename U, std::size_t N>
inline
enable_if<
  has_greater<T,U,bool>,
  bool>
operator>(const aggregate<T,N>& a, const aggregate<U,N>& b)
{ return b < a; }

template <typename T, typename U, std::size_t N>
inline
enable_if<
  has_less_equal<T,U,bool>,
  bool>
operator<=(const aggregate<T,N>& a, const aggregate<U,N>& b)
{ return !(a > b); }

template <typename T, typename U, std::size_t N>
inline
enable_if<
  has_less_equal<T,U,bool>,
  bool>
operator>=(const aggregate<T,N>& a, const aggregate<U,N>& b)
{ return !(a < b); }



//! NOTE: explicit SFINAE in return type of mutating binary operators
//! TODO? If the return type of += is NOT T&,
//!       then return a aggregate<result,N> instead of a aggregate<T,N>&
//!       The underlying types are being clever with expression templates or such
//! ALSO: How does aggregate<aggregate<T,N>,M> interact?

/**  TODO?
  if result<T&> == void
    return aggregate<T,N>& [original]

  if result == R
    return aggregate<R,N>

  if result == T&
    return aggregate<T,N>&

  if result == const T&
    return const aggregate<T,N>&
 */

#define AGG_UN_OP_ASSIGN(NAME,OP)                                             \
  template <typename T, std::size_t N>                                        \
  inline                                                                      \
  enable_if<                                                                  \
    has_##NAME<T&>,                                                           \
    aggregate<T,N>&>                                                          \
  operator OP(aggregate<T,N>& a) {                                            \
    detail::tuple_map<void>(fn::NAME(), a);                                   \
    return a;                                                                 \
  }

AGG_UN_OP_ASSIGN(pre_increment, ++)
AGG_UN_OP_ASSIGN(pre_decrement, --)
#undef AGG_UN_OP_ASSIGN

#define AGG_UN_OP(NAME,OP)                                                    \
  template <typename T, std::size_t N>                                        \
  inline                                                                      \
  enable_if<                                                                  \
    has_##NAME<T&>,                                                           \
    aggregate<NAME##_result_t<T&>,N> >                                        \
  operator OP(aggregate<T,N>& a) {                                            \
    return detail::tuple_map<aggregate<NAME##_result_t<T&>,N>>(fn::NAME(), a);\
  }

AGG_UN_OP(unary_plus,       +)
AGG_UN_OP(unary_minus,      -)
AGG_UN_OP(bit_not,          ~)
AGG_UN_OP(logical_not,      !)
AGG_UN_OP(dereference,      *)
AGG_UN_OP(address_of,       &)
#undef AGG_UN_OP

#define AGG_UN_OP(NAME,OP)                                                    \
  template <typename T, std::size_t N>                                        \
  inline                                                                      \
  enable_if<                                                                  \
    has_##NAME<T&>,                                                           \
    aggregate<NAME##_result_t<T&>,N> >                                        \
  operator OP(aggregate<T,N>& a, int) {                                       \
    return detail::tuple_map<aggregate<NAME##_result_t<T&>,N>>(fn::NAME(), a);\
  }

AGG_UN_OP(post_increment,  ++)
AGG_UN_OP(post_decrement,  --)
#undef AGG_UN_OP


#define AGG_BIN_OP_ASSIGN(NAME,OP)                                            \
  template <typename T, typename U, std::size_t N>                            \
  inline                                                                      \
  enable_if<                                                                  \
    has_##NAME<T&,const U&>,                                                  \
    aggregate<T,N>&>                                                          \
  operator OP(aggregate<T,N>& a, const aggregate<U,N>& b) {                   \
    detail::tuple_map<void>(fn::NAME(), a, b);                                \
    return a;                                                                 \
  }                                                                           \
  template <typename T, typename U, std::size_t N>                            \
  inline                                                                      \
  enable_if<                                                                  \
    has_##NAME<T&,const U&>,                                                  \
    aggregate<T,N>&>                                                          \
  operator OP(aggregate<T,N>& a, const U& b) {                                \
    detail::tuple_map<void>([&](T& t) { return fn::NAME()(t,b); }, a);        \
    return a;                                                                 \
  }

AGG_BIN_OP_ASSIGN(plus_assign,         +=)
AGG_BIN_OP_ASSIGN(minus_assign,        -=)
AGG_BIN_OP_ASSIGN(multiplies_assign,   *=)
AGG_BIN_OP_ASSIGN(divides_assign,      /=)
AGG_BIN_OP_ASSIGN(modulus_assign,      %=)
AGG_BIN_OP_ASSIGN(bit_and_assign,      &=)
AGG_BIN_OP_ASSIGN(bit_or_assign,       |=)
AGG_BIN_OP_ASSIGN(bit_xor_assign,      ^=)
AGG_BIN_OP_ASSIGN(left_shift_assign,  <<=)
AGG_BIN_OP_ASSIGN(right_shift_assign, >>=)
#undef AGG_BIN_OP_ASSIGN


#define AGG_BIN_OP(NAME,OP)                                                   \
  template <typename T, typename U, std::size_t N>                            \
  inline                                                                      \
  enable_if<                                                                  \
    has_##NAME<T,U>,                                                          \
    aggregate<NAME##_result_t<T,U>,N> >                                       \
  operator OP(const aggregate<T,N>& a, const aggregate<U,N>& b) {             \
    using R = aggregate<NAME##_result_t<T,U>,N>;                              \
    return detail::tuple_map<R>(fn::NAME(), a, b);                            \
  }                                                                           \
  template <typename T, typename U, std::size_t N>                            \
  inline                                                                      \
  enable_if<                                                                  \
    has_##NAME<T,U>,                                                          \
    aggregate<NAME##_result_t<T,U>,N> >                                       \
  operator OP(const aggregate<T,N>& a, const U& b) {                          \
    using R = aggregate<NAME##_result_t<T,U>,N>;                              \
    return detail::tuple_map<R>([&](const T& t){return fn::NAME()(t,b);}, a); \
  }                                                                           \
  template <typename T, typename U, std::size_t N>                            \
  inline                                                                      \
  enable_if<                                                                  \
    has_##NAME<T,U>,                                                          \
    aggregate<NAME##_result_t<T,U>,N> >                                       \
  operator OP(const T& a, const aggregate<U,N>& b) {                          \
    using R = aggregate<NAME##_result_t<T,U>,N>;                              \
    return detail::tuple_map<R>([&](const U& u){return fn::NAME()(a,u);}, b); \
  }

#define COMMA ,

AGG_BIN_OP(plus,                 +)
AGG_BIN_OP(minus,                -)
AGG_BIN_OP(multiplies,           *)
AGG_BIN_OP(divides,              /)
AGG_BIN_OP(modulus,              %)
AGG_BIN_OP(bit_and,              &)
AGG_BIN_OP(bit_or,               |)
AGG_BIN_OP(bit_xor,              ^)
AGG_BIN_OP(comma,            COMMA)
AGG_BIN_OP(left_shift,          <<)
AGG_BIN_OP(right_shift,         >>)
AGG_BIN_OP(logical_and,         &&)
AGG_BIN_OP(logical_or,          ||)
#undef AGG_BIN_OP
#undef COMMA

} // end namespace agg
