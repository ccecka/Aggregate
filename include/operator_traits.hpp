#pragma once

#include <utility>

#define COMMA ,

namespace agg {

/** C++14-like <functional> extensions */
namespace fn {

#define LEFT_UNARY_OP(NAME,OP)                                                \
  struct NAME {                                                               \
    template <class T>                                                        \
    constexpr auto operator()(T&& arg) const                                  \
        -> decltype( OP std::forward<T>(arg)) {                               \
      return OP std::forward<T>(arg);                                         \
    }                                                                         \
  }

#define RIGHT_UNARY_OP(NAME,OP)                                               \
  struct NAME {                                                               \
    template <class T>                                                        \
    constexpr auto operator()(T&& arg) const                                  \
        -> decltype(std::forward<T>(arg) OP ) {                               \
      return std::forward<T>(arg) OP ;                                        \
    }                                                                         \
  }

#define BINARY_OP(NAME,OP)                                                    \
  struct NAME {                                                               \
    template <class T, class U>                                               \
    constexpr auto operator()(T&& lhs, U&& rhs) const                         \
        -> decltype(std::forward<T>(lhs) OP std::forward<U>(rhs)) {           \
      return std::forward<T>(lhs) OP std::forward<U>(rhs);                    \
    }                                                                         \
  }

BINARY_OP(plus,                 +);
BINARY_OP(minus,                -);
BINARY_OP(multiplies,           *);
BINARY_OP(divides,              /);
BINARY_OP(assign,               =);
BINARY_OP(modulus,              %);
BINARY_OP(bit_and,              &);
BINARY_OP(bit_or,               |);
BINARY_OP(bit_xor,              ^);
BINARY_OP(comma,            COMMA);
BINARY_OP(left_shift,          <<);
BINARY_OP(right_shift,         >>);

BINARY_OP(plus_assign,         +=);
BINARY_OP(minus_assign,        -=);
BINARY_OP(multiplies_assign,   *=);
BINARY_OP(divides_assign,      /=);
BINARY_OP(modulus_assign,      %=);
BINARY_OP(bit_and_assign,      &=);
BINARY_OP(bit_or_assign,       |=);
BINARY_OP(bit_xor_assign,      ^=);
BINARY_OP(left_shift_assign,  <<=);
BINARY_OP(right_shift_assign, >>=);

BINARY_OP(logical_and,         &&);
BINARY_OP(logical_or,          ||);
BINARY_OP(equal_to,            ==);
BINARY_OP(not_equal_to,        !=);
BINARY_OP(greater,              >);
BINARY_OP(less,                 <);
BINARY_OP(greater_equal,       >=);
BINARY_OP(less_equal,          <=);

LEFT_UNARY_OP(unary_plus,       +);
LEFT_UNARY_OP(unary_minus,      -);
LEFT_UNARY_OP(bit_not,          ~);
LEFT_UNARY_OP(logical_not,      !);
LEFT_UNARY_OP(dereference,      *);
LEFT_UNARY_OP(address_of,       &);
LEFT_UNARY_OP(pre_increment,   ++);
LEFT_UNARY_OP(pre_decrement,   --);

RIGHT_UNARY_OP(post_increment, ++);
RIGHT_UNARY_OP(post_decrement, --);

// Subscript Operator
struct subscript {
  template <class T, class U>
  constexpr auto operator()(T&& lhs, U&& rhs) const
      -> decltype(std::forward<T>(lhs) [ std::forward<U>(rhs) ] ) {
    return std::forward<T>(lhs) [ std::forward<U>(rhs) ];
  }
};

// Function Operator
struct function {
  template <class T, class... U>
  constexpr auto operator()(T&& lhs, U&&... rhs) const
      -> decltype(std::forward<T>(lhs) ( std::forward<U>(rhs)... ) ) {
    return std::forward<T>(lhs) ( std::forward<U>(rhs)... );
  }
};

#undef LEFT_UNARY_OP
#undef RIGHT_UNARY_OP
#undef BINARY_OP

} // end namespace fn


namespace sfinae {

template <class F, class... Args>
struct invokeable {
  template <class U = F>
  static auto test(int) -> decltype(std::declval<U>()(std::declval<Args>()...),
                                    void(), std::true_type());
  static auto test(...) -> std::false_type;

  using type = decltype(test(0));
  static constexpr bool value = type::value;
};

template <bool Callable, class ReturnType, class F, class... Args>
struct check_return
    : std::is_convertible<decltype(std::declval<F>()(std::declval<Args>()...)),
                          ReturnType>::type {
};

template <class ReturnType, class F, class... Args>
struct check_return<false, ReturnType, F, Args...>
    : std::false_type {
};


struct __ignore {};

template <class ReturnType, class F, class... Args>
struct check_function
    : check_return<invokeable<F, Args...>::value, ReturnType, F, Args...>::type {
};

template <class F, class... Args>
struct check_function<__ignore, F, Args...>
    : invokeable<F, Args...>::type {
};

} // end namespace sfinae



#define BINARY_TRAIT(NAME, OP)                                                \
  template <class T1, class T2 = T1, class Result = sfinae::__ignore>         \
  struct has_##NAME : sfinae::check_function<Result,fn::NAME,T1,T2>::type{};  \
                                                                              \
  template <class T1, class T2 = T1>                                          \
  using NAME##_result_t = decltype(std::declval<T1>() OP std::declval<T2>())

#define LEFT_UNARY_TRAIT(NAME, OP)                                            \
  template <class T1, class Result = sfinae::__ignore>                        \
  struct has_##NAME : sfinae::check_function<Result,fn::NAME,T1>::type{};     \
                                                                              \
  template <class T1>                                                         \
  using NAME##_result_t = decltype(OP std::declval<T1>())

#define RIGHT_UNARY_TRAIT(NAME, OP)                                           \
  template <class T1, class Result = sfinae::__ignore>                        \
  struct has_##NAME : sfinae::check_function<Result,fn::NAME,T1>::type{};     \
                                                                              \
  template <class T1>                                                         \
  using NAME##_result_t = decltype(std::declval<T1>() OP)


BINARY_TRAIT(plus,                 +);
BINARY_TRAIT(minus,                -);
BINARY_TRAIT(multiplies,           *);
BINARY_TRAIT(divides,              /);
BINARY_TRAIT(assign,               =);
BINARY_TRAIT(modulus,              %);
BINARY_TRAIT(bit_and,              &);
BINARY_TRAIT(bit_or,               |);
BINARY_TRAIT(bit_xor,              ^);
BINARY_TRAIT(comma,            COMMA);
BINARY_TRAIT(left_shift,          <<);
BINARY_TRAIT(right_shift,         >>);

BINARY_TRAIT(plus_assign,         +=);
BINARY_TRAIT(minus_assign,        -=);
BINARY_TRAIT(multiplies_assign,   *=);
BINARY_TRAIT(divides_assign,      /=);
BINARY_TRAIT(modulus_assign,      %=);
BINARY_TRAIT(bit_and_assign,      &=);
BINARY_TRAIT(bit_or_assign,       |=);
BINARY_TRAIT(bit_xor_assign,      ^=);
BINARY_TRAIT(left_shift_assign,  <<=);
BINARY_TRAIT(right_shift_assign, >>=);

BINARY_TRAIT(logical_and,         &&);
BINARY_TRAIT(logical_or,          ||);
BINARY_TRAIT(equal_to,            ==);
BINARY_TRAIT(not_equal_to,        !=);
BINARY_TRAIT(greater,              >);
BINARY_TRAIT(less,                 <);
BINARY_TRAIT(greater_equal,       >=);
BINARY_TRAIT(less_equal,          <=);

LEFT_UNARY_TRAIT(unary_plus,       +);
LEFT_UNARY_TRAIT(unary_minus,      -);
LEFT_UNARY_TRAIT(bit_not,          ~);
LEFT_UNARY_TRAIT(logical_not,      !);
LEFT_UNARY_TRAIT(dereference,      *);
LEFT_UNARY_TRAIT(address_of,       &);
LEFT_UNARY_TRAIT(pre_increment,   ++);
LEFT_UNARY_TRAIT(pre_decrement,   --);

RIGHT_UNARY_TRAIT(post_increment, ++);
RIGHT_UNARY_TRAIT(post_decrement, --);

// Subscript Operator
template <class T1, class T2 = T1, class Result = sfinae::__ignore>
struct has_subscript : sfinae::check_function<Result,fn::subscript,T1,T2>::type{};

template <class T1, class T2 = T1>
using subscript_result_t = decltype(std::declval<T1>()[std::declval<T2>()]);

// Function Operator
template <class T1, class... T2>
struct has_function : sfinae::check_function<sfinae::__ignore,
                                             fn::function,T1,T2...>::type{};

template <class T1, class... T2>
using function_result_t = decltype(std::declval<T1>()(std::declval<T2>()...));


#undef BINARY_TRAIT
#undef LEFT_UNARY_TRAIT
#undef RIGHT_UNARY_TRAIT

} // end namespace agg

#undef COMMA
