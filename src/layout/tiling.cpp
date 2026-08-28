#include "tiling.hpp"

#include <algorithm>
#include <cmath>

namespace lewm {

namespace {

struct Rect {
    int x, y, w, h;
};

void place(WindowAnimator& anim, const Config& cfg, Louvre::LSurface* s, const Rect& r) {
    Louvre::LPoint from = s->pos();
    Louvre::LPoint target(r.x, r.y);
    if (from != target)
        anim.move(s, from, target, cfg.anim.move_ms, cfg.anim.easing);
}

void dwindl(const std::vector<Louvre::LSurface*>& wins, int from, int n,
            Rect rect, bool horiz, int gap, WindowAnimator& anim, const Config& cfg) {
    if (from >= n) return;
    if (from == n - 1) {
        place(anim, cfg, wins[from], rect);
        return;
    }
    Louvre::LSurface* m = wins[from];
    if (horiz) {
        int mainW = std::max<int>(rect.w * 2 / 5, rect.w / 3);
        place(anim, cfg, m, Rect{ rect.x, rect.y, mainW, rect.h });
        Rect rest = { rect.x + mainW + gap, rect.y, rect.w - mainW - gap, rect.h };
        dwindl(wins, from + 1, n, rest, false, gap, anim, cfg);
    } else {
        int mainH = std::max<int>(rect.h * 2 / 5, rect.h / 3);
        place(anim, cfg, m, Rect{ rect.x, rect.y, rect.w, mainH });
        Rect rest = { rect.x, rect.y + mainH + gap, rect.w, rect.h - mainH - gap };
        dwindl(wins, from + 1, n, rest, true, gap, anim, cfg);
    }
}

} // namespace

Layout layout_from_name(const std::string& name) {
    if (name == "grid") return Layout::Grid;
    if (name == "dwindle") return Layout::Dwindle;
    if (name == "master") return Layout::Master;
    if (name == "custom") return Layout::Custom;
    return Layout::Tile;
}

const char* layout_name(Layout layout) {
    switch (layout) {
        case Layout::Grid: return "grid";
        case Layout::Dwindle: return "dwindle";
        case Layout::Master: return "master";
        case Layout::Custom: return "custom";
        default: return "tile";
    }
}

void relayout(Layout kind,
              Louvre::LOutput* output,
              const std::vector<Louvre::LSurface*>& windows,
              WindowAnimator& anim,
              const Config& cfg,
              float split_ratio,
              int gap) {
    if (!output || windows.empty()) return;
    if (kind == Layout::Custom) return;

    Louvre::LRect avail = output->availableGeometry();
    int n = (int)windows.size();

    if (kind == Layout::Tile) {
        int h = (avail.h() - gap * (n - 1)) / n;
        int y = avail.y();
        for (Louvre::LSurface* s : windows) {
            place(anim, cfg, s, Rect{ avail.x(), y, avail.w(), h });
            y += h + gap;
        }
        return;
    }

    if (kind == Layout::Grid) {
        int cols = (int)std::ceil(std::sqrt(n));
        int rows = (int)std::ceil((float)n / cols);
        int cw = (avail.w() - gap * (cols - 1)) / cols;
        int ch = (avail.h() - gap * (rows - 1)) / rows;
        int i = 0;
        for (Louvre::LSurface* s : windows) {
            int c = i % cols;
            int r = i / cols;
            place(anim, cfg, s, Rect{ avail.x() + c * (cw + gap),
                                      avail.y() + r * (ch + gap),
                                      cw, ch });
            i++;
        }
        return;
    }

    if (kind == Layout::Dwindle) {
        dwindl(windows, 0, n, Rect{ avail.x(), avail.y(), avail.w(), avail.h() },
               avail.w() >= avail.h(), gap, anim, cfg);
        return;
    }

    if (kind == Layout::Master) {
        float ratio = std::clamp(split_ratio, 0.2f, 0.8f);
        int masterW = (int)((float)avail.w() * ratio);
        int remW = avail.w() - masterW - gap;
        place(anim, cfg, windows[0], Rect{ avail.x(), avail.y(), masterW, avail.h() });
        int remN = n - 1;
        int h = (avail.h() - gap * (remN - 1)) / remN;
        int y = avail.y();
        for (int j = 1; j < n; ++j) {
            place(anim, cfg, windows[j], Rect{ avail.x() + masterW + gap, y, remW, h });
            y += h + gap;
        }
    }
}

} // namespace lewm
