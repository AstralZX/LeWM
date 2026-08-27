#include "tiling.hpp"

#include <algorithm>
#include <cmath>

namespace lewm {

void Tiling::relayout(LayoutKind kind,
                      Louvre::LOutput* output,
                      const std::vector<Louvre::LSurface*>& windows,
                      WindowAnimator& anim,
                      const Config& cfg) {
    if (!output || windows.empty()) return;

    Louvre::LRect avail = output->availableGeometry();
    int gap = cfg.gap;
    int n = (int)windows.size();

    if (kind == LayoutKind::Grid) {
        int cols = (int)std::ceil(std::sqrt(n));
        int rows = (int)std::ceil((float)n / cols);
        int cw = (avail.w() - gap * (cols - 1)) / cols;
        int ch = (avail.h() - gap * (rows - 1)) / rows;
        int i = 0;
        for (Louvre::LSurface* s : windows) {
            int c = i % cols;
            int r = i / cols;
            Louvre::LPoint target(avail.x() + c * (cw + gap),
                                  avail.y() + r * (ch + gap));
            Louvre::LPoint from = s->pos();
            if (from == target) s->setPos(target);
            else anim.move(s, from, target, cfg.anim.move_ms, cfg.anim.easing);
            i++;
        }
        return;
    }

    // Tile: single column, equal height.
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
