#ifndef DAMATH_ZERO_BASE_ID_H
#define DAMATH_ZERO_BASE_ID_H

#include "types.h"

namespace DamathZero::Base {

class Id {
 public:
  constexpr Id(i32 value) : value_(value) {}
  constexpr Id() : value_(-1) {}

  constexpr auto is_valid() const -> bool { return value_ != -1; }
  constexpr auto value() const -> i32 { return value_; }
  static constexpr auto invalid() -> Id { return Id{-1}; }

 private:
  i32 value_;
};

}  // namespace DamathZero::Base

#endif  // !DAMATH_ZERO_BASE_INDEX_H
