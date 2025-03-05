#ifndef DAMATH_ZERO_BASE_ID_H
#define DAMATH_ZERO_BASE_ID_H

#include <glaze/core/common.hpp>

#include "damath-zero/base/types.h"

namespace DamathZero::Base {

template <typename T>
class Id {
 public:
  constexpr Id(i32 value) : value_(value) {}
  constexpr Id() : value_(-1) {}

  constexpr auto is_valid() const -> bool { return value_ != -1; }
  constexpr auto value() const -> i32 { return value_; }

 private:
  i32 value_;

 public:
  struct glaze;
};

template <typename T>
struct Id<T>::glaze {
  using U = Id<T>;

  static constexpr auto value =
      glz::object("name", &glz::name_v<T>, "value", &U::value_);
};

}  // namespace DamathZero::Base

#endif  // !DAMATH_ZERO_BASE_INDEX_H
