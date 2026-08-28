#ifndef LEWM_SCENE_HPP
#define LEWM_SCENE_HPP

#include <LScene.h>
#include <LLayerView.h>
#include <LSceneView.h>

namespace lewm {

enum LayerIndex : int {
    LayerBackground = 0,
    LayerBottom,
    LayerMiddle,
    LayerTop,
    LayerOverlay,
    LayerCount
};

class LeWMScene final : public Louvre::LScene {
public:
    LeWMScene() noexcept;

    Louvre::LLayerView layers[LayerCount];
};

} // namespace lewm

#endif
