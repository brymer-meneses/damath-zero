#pragma once

#include <glaze/core/common.hpp>

#include "damath-zero/base/types.h"

namespace DamathZero::Base {

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

struct Id::glaze {
  using T = Id;

  static constexpr auto value{&T::value_};
};

}  // namespace DamathZero::Base
