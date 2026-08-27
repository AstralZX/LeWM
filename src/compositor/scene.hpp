#ifndef LEWM_SCENE_HPP
#define LEWM_SCENE_HPP

#include <LScene.h>
#include <LLayerView.h>
#include <LSceneView.h>

namespace lewm {

// Stacking layers, bottom (background) to top (overlay).
enum LayerIndex : int {
    LayerBackground = 0,
    LayerBottom,
    LayerMiddle,
    LayerTop,
    LayerOverlay,
    LayerCount
};

// One scene drives every output. It has a main view plus the five standard
// layers (background, bottom, middle, top, overlay) that Louvre surfaces
// slot into by their role.
class LeWMScene final : public Louvre::LScene {
public:
    LeWMScene() noexcept;

    Louvre::LLayerView layers[LayerCount];
};

} // namespace lewm

#endif
