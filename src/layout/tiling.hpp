#pragma once

#include <Louvre/LOutput.h>
#include <vector>
#include "anim/animator.hpp"
#include "config/le_ast.hpp"

namespace lewm {

enum class Layout {
    Tile,
    Grid,
    Dwindle,
    Master,
    Custom
};

Layout layout_from_name(const std::string& name);
const char* layout_name(Layout layout);

void relayout(Layout kind,
              Louvre::LOutput* output,
              const std::vector<Louvre::LSurface*>& windows,
              WindowAnimator& anim,
              const Config& cfg,
              float split_ratio,
              int gap);

} // namespace lewm
