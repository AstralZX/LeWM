#ifndef LEWM_PANEL_HPP
#define LEWM_PANEL_HPP

#include <Louvre/LSceneView.h>
#include <Louvre/LLayerView.h>
#include <Louvre/LSolidColorView.h>
#include <Louvre/LColor.h>

namespace lewm {

Louvre::LRGBAF hex_to_color(const std::string& hex);

class SettingsPanel {
public:
    SettingsPanel(Louvre::LLayerView* overlay, const Louvre::LRGBAF& color);

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
