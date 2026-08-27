#ifndef LEWM_SURFACE_HPP
#define LEWM_SURFACE_HPP

#include <LSurface.h>
#include <LSurfaceView.h>

namespace lewm {

class LeWMToplevelRole;

// A client window. We keep an LSurfaceView for scene rendering and push Z
// order / layer changes into the scene so views match Louvre's surface order.
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
