#include "pointer.hpp"
#include "global.hpp"
#include "lewm_compositor.hpp"
#include "surface.hpp"

#include <Louvre/LCursor.h>
#include <Louvre/LPointerButtonEvent.h>
#include <Louvre/LPointerMoveEvent.h>
#include <Louvre/LScene.h>
#include <Louvre/LToplevelRole.h>
#include <algorithm>

namespace lewm {

namespace {
constexpr int RESIZE_EDGE = 8;

bool hitHorizontal(const Louvre::LRect& r, Louvre::LPoint p) {
    return p.y() >= r.y() - RESIZE_EDGE && p.y() <= r.y() + RESIZE_EDGE;
}
bool hitVertical(const Louvre::LRect& r, Louvre::LPoint p) {
    return p.x() >= r.x() - RESIZE_EDGE && p.x() <= r.x() + RESIZE_EDGE;
}

LeWMPointer::Op detectOp(const Louvre::LRect& r, Louvre::LPoint p) {
    bool left = p.x() <= r.x() + RESIZE_EDGE;
    bool right = p.x() >= r.x() + r.w() - RESIZE_EDGE;
    bool top = p.y() <= r.y() + RESIZE_EDGE;
    bool bottom = p.y() >= r.y() + r.h() - RESIZE_EDGE;
    (void)left;

    if (top && left) return LeWMPointer::Op::ResizeTopLeft;
    if (top && right) return LeWMPointer::Op::ResizeTopRight;
    if (bottom && left) return LeWMPointer::Op::ResizeBottomLeft;
    if (bottom && right) return LeWMPointer::Op::ResizeBottomRight;
    if (top) return LeWMPointer::Op::ResizeTop;
    if (bottom) return LeWMPointer::Op::ResizeBottom;
    if (left) return LeWMPointer::Op::ResizeLeft;
    if (right) return LeWMPointer::Op::ResizeRight;
    return LeWMPointer::Op::Move;
}
} // namespace

void LeWMPointer::pointerButtonEvent(const Louvre::LPointerButtonEvent& event) {
    LeWMCompositor& wm = self();
    Louvre::LPoint p = Louvre::cursor()->pos();

    if (event.state() == Louvre::LPointerButtonEvent::Pressed) {
        Louvre::LSurface* s = surfaceAt(p);
        if (s)
            wm.focusSurface(s);
        if (event.button() == Louvre::LPointerButtonEvent::Left && s && wm.isFloating(s)) {
            drag_surf_ = s;
            Louvre::LRect r(s->pos().x(), s->pos().y(), s->size().w(), s->size().h());
            Op d = detectOp(r, p);
            if (d == Op::Move) {
                op_ = Op::Move;
                drag_off_ = s->pos() - p;
            } else {
                op_ = d;
                start_ptr_ = p;
                start_pos_ = s->pos();
                start_w_ = s->size().w();
                start_h_ = s->size().h();
            }
        }
    } else {
        drag_surf_ = nullptr;
        op_ = Op::None;
    }

    Louvre::LPointer::pointerButtonEvent(event);
}

void LeWMPointer::pointerMoveEvent(const Louvre::LPointerMoveEvent& event) {
    if (drag_surf_) {
        if (op_ == Op::Move) {
            drag_surf_->setPos(Louvre::cursor()->pos() + drag_off_);
            if (auto* ws = static_cast<LeWMSurface*>(drag_surf_))
                ws->updateBorder();
            return;
        }

        Louvre::LPoint d = Louvre::cursor()->pos() - start_ptr_;
        int x = start_pos_.x(), y = start_pos_.y();
        int w = start_w_, h = start_h_;

        auto shrinkX = [&] { w = std::max(80, start_w_ - d.x()); x = start_pos_.x() + (start_w_ - w); };
        auto growX = [&] { w = std::max(80, start_w_ + d.x()); };
        auto shrinkY = [&] { h = std::max(80, start_h_ - d.y()); y = start_pos_.y() + (start_h_ - h); };
        auto growY = [&] { h = std::max(80, start_h_ + d.y()); };

        switch (op_) {
            case Op::ResizeLeft: shrinkX(); break;
            case Op::ResizeRight: growX(); break;
            case Op::ResizeTop: shrinkY(); break;
            case Op::ResizeBottom: growY(); break;
            case Op::ResizeTopLeft: shrinkX(); shrinkY(); break;
            case Op::ResizeTopRight: growX(); shrinkY(); break;
            case Op::ResizeBottomLeft: shrinkX(); growY(); break;
            case Op::ResizeBottomRight: growX(); growY(); break;
            default: break;
        }

        drag_surf_->setPos({ x, y });
        if (auto* tl = drag_surf_->toplevel())
            tl->configureSize(w, h);
        if (auto* ws = static_cast<LeWMSurface*>(drag_surf_))
            ws->updateBorder();
        return;
    }

    Louvre::LPointer::pointerMoveEvent(event);
}

} // namespace lewm
