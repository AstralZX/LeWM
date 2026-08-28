#pragma once

#include <LPointer.h>
#include <Louvre/LPoint.h>

namespace lewm {

class LeWMPointer final : public Louvre::LPointer {
public:
    using LPointer::LPointer;

    void pointerButtonEvent(const Louvre::LPointerButtonEvent& event) override;
    void pointerMoveEvent(const Louvre::LPointerMoveEvent& event) override;

    enum class Op {
        None,
        Move,
        ResizeLeft,
        ResizeRight,
        ResizeTop,
        ResizeBottom,
        ResizeTopLeft,
        ResizeTopRight,
        ResizeBottomLeft,
        ResizeBottomRight,
    };

private:
    Louvre::LSurface* drag_surf_ = nullptr;
    Louvre::LPoint drag_off_;
    Op op_ = Op::None;
    Louvre::LPoint start_ptr_;
    Louvre::LPoint start_pos_;
    int start_w_ = 0;
    int start_h_ = 0;
};

} // namespace lewm
