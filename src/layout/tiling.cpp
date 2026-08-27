#include "tiling.hpp"

#include <algorithm>

namespace le {

Tiler::Tiler() = default;

void Tiler::add_window(const std::string& id) {
    if (std::find(order_.begin(), order_.end(), id) == order_.end())
        order_.push_back(id);
}

void Tiler::remove_window(const std::string& id) {
    auto it = std::find(order_.begin(), order_.end(), id);
    if (it != order_.end()) order_.erase(it);
}

void Tiler::focus(const std::string& id) {
    auto it = std::find(order_.begin(), order_.end(), id);
    if (it != order_.end() && it != order_.begin()) {
        std::rotate(order_.begin(), it, it + 1);
    }
}

void Tiler::cycle_layout() {
    auto it = std::find(layouts_.begin(), layouts_.end(), layout_);
    layout_ = (it + 1 == layouts_.end()) ? layouts_.front() : *(it + 1);
}

} // namespace le

} // namespace le
