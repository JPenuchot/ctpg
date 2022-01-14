#pragma once

#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <utility>

namespace ctpg::stdex {

template <typename T>
struct is_cqueue_compatible
    : std::bool_constant<std::is_default_constructible_v<T> &&
                         std::is_trivially_destructible_v<T>> {};

template <typename T, std::size_t N, typename = void> class cqueue {};

template <typename T, std::size_t N>
class cqueue<T, N, std::enable_if_t<is_cqueue_compatible<T>::value>> {
public:
  constexpr cqueue() : data{}, start{0}, end{0}, size_{0} {}

  constexpr void push(const T &v) {
    if (size_ >= N)
      throw std::runtime_error("Pushing out of range");

    data[end++] = v;
    if (end == N)
      end = 0;
    size_++;
  }

  constexpr void pop() {
    if (empty())
      throw std::runtime_error("Pop on empty");

    start++;
    if (start == N)
      start = 0;
    size_--;
  }

  constexpr T &top() {
    if (empty())
      throw std::runtime_error("Top on empty");
    return data[start];
  }

  constexpr const T &top() const { return std::as_const(*this).top(); }

  constexpr bool empty() const { return size_ == 0; }

  constexpr std::size_t size() const { return size_; }

private:
  T data[N];
  std::size_t start;
  std::size_t end;
  std::size_t size_;
};

} // namespace ctpg::stdex
