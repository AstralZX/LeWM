#include "scene.hpp"

namespace lewm {

LeWMScene::LeWMScene() noexcept : Louvre::LScene() {
    mainView()->setClearColor({ .r = 0.01f, .g = 0.02f, .b = 0.02f, .a = 1.f });
    for (int i = 0; i <= Louvre::LLayerOverlay; ++i)
        layers[i].setParent(mainView());
}

} // namespace lewm
