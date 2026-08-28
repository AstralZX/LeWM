#ifndef LEWM_SURFACE_HPP
#define LEWM_SURFACE_HPP

#include <LSurface.h>
#include <LSurfaceView.h>
#include <LSolidColorView.h>

namespace lewm {

class LeWMToplevelRole;

class LeWMSurface final : public Louvre::LSurface {
public:
    LeWMSurface(const void* params);

    Louvre::LSurfaceView view { this };
    Louvre::LSolidColorView bTop, bBottom, bLeft, bRight;
    Louvre::LView* getView() noexcept { return &view; }

    void updateBorder();
    void setBorderVisible(bool v);

    void roleChanged() override;
    void layerChanged() override;
    void orderChanged() override;
    void mappingChanged() override;
    void minimizedChanged() override;
    void sizeChanged() override;
};

} // namespace lewm

#endif
