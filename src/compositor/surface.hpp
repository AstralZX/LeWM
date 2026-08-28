#ifndef LEWM_SURFACE_HPP
#define LEWM_SURFACE_HPP

#include <LSurface.h>
#include <LSurfaceView.h>

namespace lewm {

class LeWMToplevelRole;

class LeWMSurface final : public Louvre::LSurface {
public:
    using LSurface::LSurface;

    Louvre::LSurfaceView view { this };

    Louvre::LView* getView() noexcept { return &view; }

    void roleChanged() override;
    void layerChanged() override;
    void orderChanged() override;
    void mappingChanged() override;
    void minimizedChanged() override;
};

} // namespace lewm

#endif
