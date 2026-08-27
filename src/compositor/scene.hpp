#ifndef LEWM_SCENE_HPP
#define LEWM_SCENE_HPP

#include <LScene.h>
#include <LLayerView.h>

namespace lewm {

// One scene drives every output. It has a main view plus the five standard
// layers (background, bottom, middle, top, overlay) that Louvre surfaces
// slot into by their role.
class LeWMScene final : public Louvre::LScene {
public:
    LeWMScene() noexcept;

    Louvre::LLayerView layers[5];
};

} // namespace lewm

#endif
