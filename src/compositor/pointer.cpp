#include "pointer.hpp"
#include "global.hpp"
#include "lewm_compositor.hpp"
#include "surface.hpp"

#include <Louvre/LCursor.h>
#include <Louvre/LPointerButtonEvent.h>
#include <Louvre/LPointerMoveEvent.h>
#include <Louvre/LScene.h>

namespace lewm {

void LeWMPointer::pointerButtonEvent(const Louvre::LPointerButtonEvent& event) {
    LeWMCompositor& wm = self();
    Louvre::LPoint p = Louvre::cursor()->pos();

    if (event.state() == Louvre::LPointerButtonEvent::Pressed) {
        Louvre::LSurface* s = surfaceAt(p);
        if (s)
            wm.focusSurface(s);
        if (event.button() == Louvre::LPointerButtonEvent::Left && s && wm.isFloating(s)) {
            drag_surf_ = s;
            drag_off_ = s->pos() - p;
        }
    } else {
        drag_surf_ = nullptr;
    }

    Louvre::LPointer::pointerButtonEvent(event);
}

void LeWMPointer::pointerMoveEvent(const Louvre::LPointerMoveEvent& event) {
    if (drag_surf_) {
        drag_surf_->setPos(Louvre::cursor()->pos() + drag_off_);
        return;
    }

    Louvre::LPointer::pointerMoveEvent(event);
}

} // namespace lewm
