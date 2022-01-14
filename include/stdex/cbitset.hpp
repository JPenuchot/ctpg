#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace ctpg::stdex {
template <std::size_t N> class cbitset {
public:
  using size_type = std::size_t;
  using underlying_type = std::uint64_t;

  constexpr cbitset &set(size_type idx) {
    check_idx(idx);
    data[idx / underlying_size] |=
        (underlying_type(1) << (idx % underlying_size));
    return *this;
  }

  constexpr cbitset &set(size_type idx, bool value) {
    check_idx(idx);
    data[idx / underlying_size] ^=
        (-!!value ^ data[idx / underlying_size]) &
        (underlying_type(1) << (idx % underlying_size));
    return *this;
  }

  constexpr cbitset &reset(size_type idx) {
    check_idx(idx);
    data[idx / underlying_size] &=
        ~(underlying_type(1) << (idx % underlying_size));
    return *this;
  }

  constexpr cbitset &flip(size_type idx) {
    check_idx(idx);
    data[idx / underlying_size] ^=
        (underlying_type(1) << (idx % underlying_size));
    return *this;
  }

  constexpr bool test(size_type idx) const {
    check_idx(idx);
    return (data[idx / underlying_size] >> (idx % underlying_size)) &
           underlying_type(1);
  }

  constexpr cbitset &flip() {
    for (auto &d : data)
      d = ~d;
    return *this;
  }

  constexpr cbitset &set() {
    for (auto &d : data)
      d = underlying_type(-1);
    return *this;
  }

  constexpr cbitset &reset() {
    for (auto &d : data)
      d = underlying_type(0);
    return *this;
  }

  constexpr size_type size() const { return N; }

private:
  constexpr void check_idx(size_type idx) const {
    if (idx >= N)
      throw std::runtime_error("Index access out of range");
  }

  static const size_type underlying_size = sizeof(underlying_type) * 8;
  static const size_type underlying_count =
      (N / underlying_size) + ((N % underlying_size) ? 1 : 0);
  underlying_type data[underlying_count] = {};
};

} // namespace ctpg::stdex
