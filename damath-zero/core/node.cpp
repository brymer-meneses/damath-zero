#include "damath-zero/core/node.h"

#include <print>

using namespace DamathZero::Core;

auto NodeStorage::print_tree(NodeId root_id, i32 level) -> void {
    for (int i = 0; i < level; ++i) {
        std::print("\t|\t");
    }

    const auto& root = get(root_id);
    std::println("{} : {} {} {}", root.action_taken.value(), root.value_sum, root.prior, root.visit_count);

    for (const auto& child_id : root.children) {
        print_tree(child_id, level + 1);
    }
}
