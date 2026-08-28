#include "surface.hpp"
#include "global.hpp"
#include "toplevel.hpp"
#include "settings/panel.hpp"

#include <Louvre/LSeat.h>
#include <Louvre/LKeyboard.h>

namespace lewm {

LeWMSurface::LeWMSurface(const void* params)
    : LSurface(params),
      bTop(0.f, 0.f, 0.f, 1.f, &view),
      bBottom(0.f, 0.f, 0.f, 1.f, &view),
      bLeft(0.f, 0.f, 0.f, 1.f, &view),
      bRight(0.f, 0.f, 0.f, 1.f, &view) {}

void LeWMSurface::setBorderVisible(bool v) {
    bTop.setVisible(v);
    bBottom.setVisible(v);
    bLeft.setVisible(v);
    bRight.setVisible(v);
}

void LeWMSurface::updateBorder() {
    if (!mapped() || !toplevel()) {
        setBorderVisible(false);
        return;
    }
    const int bw = self().settings.cfg.border_width;
    if (bw <= 0) {
        setBorderVisible(false);
        return;
    }

    const int w = size().w();
    const int h = size().h();

    Louvre::LRGBAF base = (self().seat()->keyboard()->focus() == this)
        ? hex_to_color(self().settings.cfg.border_active)
        : hex_to_color(self().settings.cfg.border_normal);
    Louvre::LRGBF color { base.r, base.g, base.b };

    auto apply = [&](Louvre::LSolidColorView& strip, int x, int y, int sw, int sh) {
        strip.setPos(x, y);
        strip.setSize(sw, sh);
        strip.setColor(color);
        strip.setVisible(true);
    };

    apply(bTop,    -bw, -bw, w + 2 * bw, bw);
    apply(bBottom, -bw, h,   w + 2 * bw, bw);
    apply(bLeft,   -bw, 0,   bw, h);
    apply(bRight,   w,  0,   bw, h);
}

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
        bTop.setParent(&view);
        bBottom.setParent(&view);
        bLeft.setParent(&view);
        bRight.setParent(&view);
        self().onSurfaceMapped(this);
        updateBorder();
    } else if (!mapped()) {
        setBorderVisible(false);
        self().onSurfaceUnmapped(this);
    }
}

void LeWMSurface::sizeChanged() {
    LSurface::sizeChanged();
    updateBorder();
}

void LeWMSurface::minimizedChanged() {
    view.setVisible(!minimized());
    if (minimized())
        raise();
}

} // namespace lewm
