#pragma once

#include <LPointer.h>
#include <Louvre/LPoint.h>

namespace lewm {

class LeWMPointer final : public Louvre::LPointer {
public:
    using LPointer::LPointer;

    void pointerButtonEvent(const Louvre::LPointerButtonEvent& event) override;
    void pointerMoveEvent(const Louvre::LPointerMoveEvent& event) override;

private:
    Louvre::LSurface* drag_surf_ = nullptr;
    Louvre::LPoint drag_off_;
};

} // namespace lewm
