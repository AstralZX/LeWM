#ifndef LEWM_PANEL_HPP
#define LEWM_PANEL_HPP

#include <Louvre/LSceneView.h>
#include <Louvre/LLayerView.h>
#include <Louvre/LSolidColorView.h>
#include <Louvre/LColor.h>

namespace lewm {

// Converts "#rrggbb" / "#rrggbbaa" to a Louvre color.
Louvre::LColor hex_to_color(const std::string& hex);

// The built-in settings panel. Right now it is a dimming overlay rendered in
// the top layer; the configuration behind it (Settings) is fully live and
// persisted. Controls get drawn on top of this once we wire up a UI toolkit.
class SettingsPanel {
public:
    SettingsPanel(Louvre::LLayerView* overlay, const Louvre::LColor& color);

    void toggle();
    void setGeometry(const Louvre::LRect& screen);
    bool visible() const { return visible_; }

private:
    Louvre::LSolidColorView view_;
    Louvre::LLayerView* overlay_;
    bool visible_ = false;
};

} // namespace lewm

#endif
