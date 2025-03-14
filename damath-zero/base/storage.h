#pragma once

#include <concepts>
#include <utility>
#include <vector>

#include "damath-zero/base/id.h"

namespace DamathZero::Base {

template <typename IdType, typename ValueType>
  requires std::derived_from<IdType, Id>
class Storage {
 public:
  auto get(IdType id) -> ValueType& {
    assert(id.is_valid());
    return data_[id.value()];
  }

  auto get(IdType id) const -> ValueType const& {
    assert(id.is_valid());
    return data_[id.value()];
  }

  template <typename... Args>
  auto create(Args&&... args) -> IdType {
    data_.emplace_back(std::forward<Args>(args)...);
    return IdType(data_.size() - 1);
  }

  auto clear() -> void { data_.clear(); }

 private:
  std::vector<ValueType> data_;
};

}  // namespace DamathZero::Base
