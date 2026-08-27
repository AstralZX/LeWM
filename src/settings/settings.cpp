#include "settings.hpp"
#include "config/le_parser.hpp"

#include <algorithm>

namespace lewm {

void Settings::load(const std::string& path) {
    cfg = load_config(path);
}

void Settings::save(const std::string& path) const {
    save_config(cfg, path);
}

bool Settings::set(const std::string& key, const std::string& value) {
    Config& c = cfg;
    auto to_bool = [&](bool& dst) {
        dst = (value == "true" || value == "1");
        return true;
    };
    auto to_int = [&](int& dst) {
        try { dst = std::stoi(value); return true; }
        catch (...) { return false; }
    };

    if (key == "gap")                  return to_int(c.gap);
    if (key == "border_width")         return to_int(c.border_width);
    if (key == "default_layout")       { c.default_layout = value; return true; }
    if (key == "modkey")               { c.modkey = value; return true; }
    if (key == "xwayland")             return to_bool(c.xwayland);
    if (key == "border_color_active")  { c.border_active = value; return true; }
    if (key == "border_color_normal")  { c.border_normal = value; return true; }
    if (key == "border_color_urgent")  { c.border_urgent = value; return true; }

    if (key == "anim.enabled")         return to_bool(c.anim.enabled);
    if (key == "anim.open_ms")         return to_int(c.anim.open_ms);
    if (key == "anim.close_ms")        return to_int(c.anim.close_ms);
    if (key == "anim.move_ms")         return to_int(c.anim.move_ms);
    if (key == "anim.easing")          { c.anim.easing = value; return true; }

    if (key == "panel.enabled")        return to_bool(c.panel.enabled);
    if (key == "panel.height")         return to_int(c.panel.height);
    if (key == "panel.position")       { c.panel.position = value; return true; }
    if (key == "panel.bg")             { c.panel.bg = value; return true; }
    if (key == "panel.fg")             { c.panel.fg = value; return true; }

    return false;
}

} // namespace lewm
