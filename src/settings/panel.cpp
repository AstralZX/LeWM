#include "panel.hpp"

#include <cstdlib>
#include <cstdint>

namespace lewm {

Louvre::LRGBAF hex_to_color(const std::string& hex) {
    Louvre::LRGBAF c { 0.f, 0.f, 0.f, 1.f };
    if (hex.size() < 7 || hex[0] != '#') return c;
    auto byte = [&](const char* p) -> float {
        return (float)std::strtoul(p, nullptr, 16) / 255.f;
    };
    c.r = byte(&hex[1]);
    c.g = byte(&hex[3]);
    c.b = byte(&hex[5]);
    if (hex.size() >= 9) c.a = byte(&hex[7]);
    return c;
}

SettingsPanel::SettingsPanel(Louvre::LLayerView* overlay, const Louvre::LRGBAF& color)
    : view_(color.r, color.g, color.b, color.a, nullptr), overlay_(overlay) {
    if (overlay_) view_.setParent(overlay_);
    view_.setVisible(false);
}

void SettingsPanel::setGeometry(const Louvre::LRect& screen) {
    view_.setPos(screen.pos());
    view_.setSize(screen.size());
}

void SettingsPanel::toggle() {
    visible_ = !visible_;
    if (visible_) {
        view_.setOpacity(0.55f);
        view_.setVisible(true);
        view_.setParent(overlay_);
    } else {
        view_.setParent(nullptr);
        view_.setVisible(false);
    }
}

} // namespace lewm
