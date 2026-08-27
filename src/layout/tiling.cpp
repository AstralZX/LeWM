#include "tiling.hpp"

#include <algorithm>

namespace lewm {

void Tiling::relayout(Louvre::LOutput* output,
                      const std::vector<Louvre::LSurface*>& windows,
                      WindowAnimator& anim,
                      const Config& cfg) {
    if (!output || windows.empty()) return;

    Louvre::LRect avail = output->availableGeometry();
    int gap = cfg.gap;
    int n = (int)windows.size();
    int h = (avail.h() - gap * (n - 1)) / n;
    int y = avail.y();

    for (Louvre::LSurface* s : windows) {
        Louvre::LPoint target(avail.x(), y);
        Louvre::LPoint from = s->pos();
        if (from == target)
            s->setPos(target);
        else
            anim.move(s, from, target, cfg.anim.move_ms, cfg.anim.easing);
        y += h + gap;
    }
}

} // namespace lewm
