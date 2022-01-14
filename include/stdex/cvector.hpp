#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

namespace ctpg::stdex {

template <typename T>
struct is_cvector_compatible
    : std::bool_constant<std::is_default_constructible_v<T> &&
                         std::is_trivially_destructible_v<T>> {};

template <typename T, std::size_t N, typename = void> class cvector {};

template <typename T, std::size_t N>
class cvector<T, N, std::enable_if_t<is_cvector_compatible<T>::value>> {
public:
  using size_type = std::size_t;

  constexpr cvector() : the_data{}, current_size(0) {}

  constexpr cvector(const T &arg, size_t count) : the_data{}, current_size(0) {
    for (size_t i = 0; i < count; ++i)
      push_back(arg);
  }

private:
  template <typename Derived> struct iterator_base {
    using it_type = Derived;
    constexpr it_type *cast() { return static_cast<it_type *>(this); }
    constexpr const it_type *cast() const {
      return static_cast<const it_type *>(this);
    }

    friend constexpr bool operator==(const it_type &a, const it_type &b) {
      return a.cast()->ptr == b.cast()->ptr;
    }
    friend constexpr bool operator!=(const it_type &a, const it_type &b) {
      return a.cast()->ptr != b.cast()->ptr;
    }
    constexpr it_type operator-(size_type amount) const {
      return it_type{cast()->ptr - amount};
    }
    constexpr size_type operator-(const it_type &other) const {
      return size_type(cast()->ptr - other.ptr);
    }
    constexpr it_type operator+(size_type amount) const {
      return it_type{cast()->ptr + amount};
    }
    constexpr it_type operator++(int) {
      it_type it{cast()->ptr};
      ++(cast()->ptr);
      return it;
    }
    constexpr it_type &operator++() {
      ++(cast()->ptr);
      return *cast();
    }
    constexpr bool operator>(const it_type &other) const {
      return cast()->ptr > other.ptr;
    }
    constexpr bool operator<(const it_type &other) const {
      return cast()->ptr < other.ptr;
    }
  };

public:
  struct iterator : iterator_base<iterator> {
    T *ptr;
    constexpr iterator(T *ptr) : ptr(ptr) {}

    constexpr T &operator*() const { return *ptr; }
  };

  struct const_iterator : iterator_base<const_iterator> {
    const T *ptr;
    constexpr const_iterator(const T *ptr) : ptr(ptr) {}
    constexpr const T &operator*() const { return *ptr; }
  };

  constexpr const T *data() const { return the_data; }

  constexpr T *data() { return the_data; }

  constexpr size_type size() const { return current_size; }

  constexpr bool empty() const { return current_size == 0; }

  constexpr void reserve(size_type) const {};

  constexpr const T &operator[](size_type idx) const { return the_data[idx]; }

  constexpr T &operator[](size_type idx) { return the_data[idx]; }

  constexpr void push_back(const T &v) { the_data[current_size++] = v; }

  constexpr void emplace_back(T &&v) {
    the_data[current_size++] = std::move(v);
  }

  constexpr const T &front() const { return the_data[0]; }

  constexpr T &front() { return the_data[0]; }

  constexpr T &back() { return the_data[current_size - 1]; }

  constexpr const T &back() const { return the_data[current_size - 1]; }

  constexpr const_iterator begin() const { return const_iterator(the_data); }

  constexpr const_iterator end() const {
    return const_iterator(the_data + current_size);
  }

  constexpr iterator begin() { return iterator(the_data); }

  constexpr iterator end() { return iterator(the_data + current_size); }

  constexpr void clear() { current_size = 0; }

  constexpr void pop_back() { current_size--; }

  constexpr iterator erase(iterator first, iterator last) {
    if (!(first < last))
      return end();
    auto from = first < begin() ? begin() : first;
    auto to = last > end() ? end() : last;
    size_type diff = to - from;
    iterator it = to;
    while (!(it == end())) {
      *from = std::move(*it);
      ++from;
      ++it;
    }
    current_size -= diff;

    return end();
  }

private:
  T the_data[N];
  size_type current_size;
};

} // namespace ctpg::stdex
