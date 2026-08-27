#include "surface.hpp"
#include "global.hpp"
#include "toplevel.hpp"

namespace lewm {

void LeWMSurface::roleChanged() {
    if (cursorRole())
        view.setVisible(false);
}

void LeWMSurface::layerChanged() {
    getView()->setParent(&lewm::self().scene.layers[layer()]);
}

void LeWMSurface::orderChanged() {
    LeWMSurface* prev = static_cast<LeWMSurface*>(prevSurface());
    getView()->insertAfter((prev && prev->layer() == layer()) ? prev->getView() : nullptr);
}

void LeWMSurface::mappingChanged() {
    LSurface::mappingChanged();

    if (mapped() && toplevel()) {
        for (Louvre::LOutput* o : compositor()->outputs())
            lewm::self().relayout(o);
    }
}

void LeWMSurface::minimizedChanged() {
    view.setVisible(!minimized());
    if (minimized())
        raise();
}

} // namespace lewm
