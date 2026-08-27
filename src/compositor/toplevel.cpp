#include "toplevel.hpp"
#include "surface.hpp"

#include <Louvre/LOutput.h>
#include <Louvre/LCursor.h>

namespace lewm {

void LeWMToplevelRole::configureRequest() {
    Louvre::LOutput* output = Louvre::cursor()->output();

    if (output)
        configureBounds(output->availableGeometry().size());
    else
        configureBounds(0, 0);

    configureSize(0, 0);
    configureState(pendingConfiguration().state | Louvre::LToplevelRole::Activated);
    configureCapabilities(Louvre::LToplevelRole::FullscreenCap |
                          Louvre::LToplevelRole::MaximizeCap |
                          Louvre::LToplevelRole::MinimizeCap |
                          Louvre::LToplevelRole::WindowMenuCap);
}

void LeWMToplevelRole::setMinimizedRequest() {
    surface()->setMinimized(true);
}

} // namespace lewm
