#ifndef LEWM_TILING_HPP
#define LEWM_TILING_HPP

#include <Louvre/LOutput.h>
#include <vector>
#include "anim/animator.hpp"
#include "config/le_ast.hpp"

namespace lewm {

enum class LayoutKind { Tile, Grid };

// Static tiling. Relayout is animated through WindowAnimator so window moves
// are smooth instead of snapping. `kind` picks how windows are arranged.
class Tiling {
public:
    void relayout(LayoutKind kind,
                  Louvre::LOutput* output,
                  const std::vector<Louvre::LSurface*>& windows,
                  WindowAnimator& anim,
                  const Config& cfg);
};

} // namespace lewm

#endif
