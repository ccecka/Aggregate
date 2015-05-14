#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

#include "operator_traits.hpp"

namespace agg {

template <typename T, std::size_t N>
struct __aggregate_traits {
  typedef T _Type[N];

  static constexpr T&
  ref(const _Type& t, std::size_t n) noexcept {
    return const_cast<T&>(t[n]);
  }
};

template <typename T>
struct __aggregate_traits<T,0> {
  struct _Type {};

  static constexpr T&
  ref(const _Type&, std::size_t) noexcept {
    //static_assert(false, "Dereferencing zero-sized array");
    return std::declval<T&>();
  }
};


/**
 *  @brief A standard container for storing a fixed size sequence of elements.
 *
 *  @ingroup sequences
 *
 *  Meets the requirements of a <a href="tables.html#65">container</a>, a
 *  <a href="tables.html#66">reversible container</a>, and a
 *  <a href="tables.html#67">sequence</a>.
 *
 *  Sets support random access iterators.
 *
 *  @tparam  T  Type of element. Required to be a complete type.
 *  @tparam  N  Number of elements.
 */
template <typename T, std::size_t N>
struct aggregate {
  typedef  T                                      value_type;
  typedef  value_type*                            pointer;
  typedef  const value_type*                      const_pointer;
  typedef  value_type&                            reference;
  typedef  const value_type&                      const_reference;
  typedef  value_type*                            iterator;
  typedef  const value_type*                      const_iterator;
  typedef  std::size_t                            size_type;
  typedef  std::ptrdiff_t                         difference_type;
  typedef  std::reverse_iterator<iterator>        reverse_iterator;
  typedef  std::reverse_iterator<const_iterator>  const_reverse_iterator;

  // Support for zero-sized aggregates mandatory.
  typedef __aggregate_traits<T,N>                 _AT;
  typename _AT::_Type                             _elem;

  // No explicit construct/copy/destroy for aggregate type.

  void
  fill(const value_type& u)
  { std::fill_n(begin(), size(), u); }

  void
  swap(aggregate& other)
      noexcept(noexcept(swap(std::declval<T&>(), std::declval<T&>())))
  { std::swap_ranges(begin(), end(), other.begin()); }

  // Iterators.
  iterator
  begin() noexcept
  { return iterator(data()); }

  const_iterator
  begin() const noexcept
  { return const_iterator(data()); }

  iterator
  end() noexcept
  { return iterator(data() + N); }

  const_iterator
  end() const noexcept
  { return const_iterator(data() + N); }

  reverse_iterator
  rbegin() noexcept
  { return reverse_iterator(end()); }

  const_reverse_iterator
  rbegin() const noexcept
  { return const_reverse_iterator(end()); }

  reverse_iterator
  rend() noexcept
  { return reverse_iterator(begin()); }

  const_reverse_iterator
  rend() const noexcept
  { return const_reverse_iterator(begin()); }

  const_iterator
  cbegin() const noexcept
  { return const_iterator(data()); }

  const_iterator
  cend() const noexcept
  { return const_iterator(data() + N); }

  const_reverse_iterator
  crbegin() const noexcept
  { return const_reverse_iterator(end()); }

  const_reverse_iterator
  crend() const noexcept
  { return const_reverse_iterator(begin()); }

  // Capacity.
  constexpr size_type
  size() const noexcept
  { return N; }

  constexpr size_type
  max_size() const noexcept
  { return N; }

  constexpr bool
  empty() const noexcept
  { return size() == 0; }

  // Element access.
  reference
  operator[](size_type n)
  { return _AT::ref(_elem, n); }

  constexpr const_reference
  operator[](size_type n) const noexcept
  { return _AT::ref(_elem, n); }

  reference
  at(size_type n)
  { return _AT::ref(_elem, n); }

  constexpr const_reference
  at(size_type n) const
  { return _AT::ref(_elem, n); }

  reference
  front()
  { return *begin(); }

  constexpr const_reference
  front() const
  { return _AT::ref(_elem, 0); }

  reference
  back()
  { return N ? *(end() - 1) : *end(); }

  constexpr const_reference
  back() const
  { return N ? _AT::ref(_elem, N - 1) : _AT::ref(_elem, 0); }

  pointer
  data() noexcept
  { return std::addressof(_AT::ref(_elem, 0)); }

  const_pointer
  data() const noexcept
  { return std::addressof(_AT::ref(_elem, 0)); }
};

} // namespace agg


//! Specialize std:: for agg::aggregate
namespace std {

//! Specialization of std::swap
template <typename T, std::size_t N>
inline void
swap(agg::aggregate<T,N>& a, agg::aggregate<T,N>& b)
    noexcept(noexcept(a.swap(b))) {
  a.swap(b);
}

//! Specialization of std::get
template <std::size_t I, typename T, std::size_t N>
constexpr T&
get(agg::aggregate<T,N>& a) noexcept {
  static_assert(I < N, "index is out of bounds");
  return agg::__aggregate_traits<T,N>::ref(a._elem, I);
}
//! Specialization of std::get
template <std::size_t I, typename T, std::size_t N>
constexpr T&&
get(agg::aggregate<T,N>&& a) noexcept {
  static_assert(I < N, "index is out of bounds");
  return std::move(get<I>(a));
}
//! Specialization of std::get
template <std::size_t I, typename T, std::size_t N>
constexpr const T&
get(const agg::aggregate<T,N>& a) noexcept {
  static_assert(I < N, "index is out of bounds");
  return agg::__aggregate_traits<T,N>::ref(a._elem, I);
}

// Specialization of std::tuple_* interface

//! Forward declare std::tuple_size
template <typename T>
class tuple_size;
//! Specialization of std::tuple_size
template <typename T, std::size_t N>
struct tuple_size<agg::aggregate<T,N> >
    : public integral_constant<std::size_t, N> {};

//! Forward declare std::tuple_element
template <std::size_t I, typename T>
class tuple_element;
//! Specialization of std::tuple_element
template <std::size_t I, typename T, std::size_t N>
struct tuple_element<I, agg::aggregate<T,N> > {
  static_assert(I < N, "index is out of bounds");
  typedef T type;
};

} // namespace std


#include "agg_operators.hpp"
