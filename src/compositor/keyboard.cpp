#include "keyboard.hpp"
#include "global.hpp"
#include "lewm_compositor.hpp"

#include <Louvre/LKeyboardKeyEvent.h>
#include <Louvre/LScene.h>
#include <Louvre/LLauncher.h>
#include <xkbcommon/xkbcommon.h>
#include <cctype>
#include <map>
#include <set>
#include <string>

namespace lewm {

namespace {

std::string norm_key(const std::string& k) {
    if (k == "Enter") return "Return";
    if (k == "Esc") return "Escape";
    std::string out = k;
    for (auto& c : out) c = std::tolower((unsigned char)c);
    return out;
}

xkb_keysym_t target_sym(const std::string& key) {
    return xkb_keysym_from_name(norm_key(key).c_str(), XKB_KEYSYM_NO_FLAGS);
}

bool mod_active(LeWMKeyboard* kb, const std::string& name) {
    return kb->isModActive(name.c_str(), XKB_STATE_MODS_EFFECTIVE);
}

} // namespace

void LeWMKeyboard::keyEvent(const Louvre::LKeyboardKeyEvent& event) {
    LeWMCompositor& wm = self();

    if (event.state() == Louvre::LKeyboardKeyEvent::Released)
        return;

    xkb_keysym_t sym = keySymbol(event.keyCode());
    char symName[64];
    xkb_keysym_get_name(sym, symName, sizeof(symName));
    std::string got(symName);
    for (auto& c : got) c = std::tolower((unsigned char)c);

    if (wm.resize_mode) {
        if (got == "up") { wm.nudgeResize(0, 1); return; }
        if (got == "down") { wm.nudgeResize(0, -1); return; }
        if (got == "left") { wm.nudgeResize(-1, 0); return; }
        if (got == "right") { wm.nudgeResize(1, 0); return; }
        if (got == "escape" || got == "return") { wm.setResizeMode(false); return; }
    }

    for (const auto& kb : wm.settings.cfg.keys) {
        std::vector<std::string> parts;
        std::string cur;
        for (char ch : kb.combo) {
            if (ch == '+') { parts.push_back(cur); cur.clear(); }
            else cur += ch;
        }
        parts.push_back(cur);
        std::string keyTok = parts.back();
        std::set<std::string> needMods(parts.begin(), parts.end() - 1);

        if (target_sym(keyTok) != sym) continue;
        bool modsOk = true;
        for (const auto& m : needMods) {
            const std::string& name = (m == "mod") ? wm.settings.cfg.modkey : m;
            if (!mod_active(this, name)) { modsOk = false; break; }
        }
        if (!modsOk) continue;

        wm.runAction(kb);
        return;
    }

    wm.scene.handleKeyboardKeyEvent(event);
}

} // namespace lewm
