#include "surface.hpp"
#include "global.hpp"
#include "toplevel.hpp"

namespace lewm {

void LeWMSurface::roleChanged() {
    if (cursorRole())
        view.setVisible(false);
}

void LeWMSurface::layerChanged() {
    getView()->setParent(&self().scene.layers[layer()]);
}

void LeWMSurface::orderChanged() {
    LeWMSurface* prev = static_cast<LeWMSurface*>(prevSurface());
    getView()->insertAfter((prev && prev->layer() == layer()) ? prev->getView() : nullptr);
}

void LeWMSurface::mappingChanged() {
    LSurface::mappingChanged();

    if (mapped() && toplevel()) {
        self().onSurfaceMapped(this);
    } else if (!mapped()) {
        self().onSurfaceUnmapped(this);
    }
}

void LeWMSurface::minimizedChanged() {
    view.setVisible(!minimized());
    if (minimized())
        raise();
}

} // namespace lewm
