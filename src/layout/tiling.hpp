#ifndef LEWM_TILING_HPP
#define LEWM_TILING_HPP

#include <Louvre/LOutput.h>
#include <Louvre/LSceneView.h>
#include <vector>
#include "anim/animator.hpp"
#include "config/le_ast.hpp"

namespace lewm {

// Static tiling. For now it lays out the mapped toplevels of an output in a
// single column, equal height, with the configured gap. Relayout is animated
// through WindowAnimator so window moves are smooth instead of snapping.
class Tiling {
public:
    void relayout(Louvre::LOutput* output,
                  const std::vector<Louvre::LSurface*>& windows,
                  WindowAnimator& anim,
                  const Config& cfg);
};

} // namespace lewm

#endif
