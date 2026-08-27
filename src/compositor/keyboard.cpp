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

// xkb keysym names are case-sensitive and "Enter" is really "Return" etc.
// Normalize a config key token to what xkb_keysym_from_name expects.
std::string norm_key(const std::string& k) {
    if (k == "Enter") return "Return";
    if (k == "Esc") return "Escape";
    std::string out = k;
    for (auto& c : out) c = std::tolower((unsigned char)c);
    return out;
}

xkb_keysym_t target_sym(const std::string& key) {
    return xkb_keysym_from_name(norm_key(key).c_str());
}

bool mod_active(LeWMKeyboard* kb, const std::string& name) {
    return kb->isModActive(name.c_str(), XKB_STATE_MODS_EFFECTIVE);
}

} // namespace

void LeWMKeyboard::keyEvent(const Louvre::LKeyboardKeyEvent& event) {
    LeWMCompositor& wm = lewm::self();

    if (event.state() == Louvre::LKeyboardKeyEvent::Pressed) {
        xkb_keysym_t sym = keySymbol(event.keyCode());
        std::string got = xkb_keysym_get_name(sym);
        for (auto& c : got) c = std::tolower((unsigned char)c);

        for (const auto& kb : wm.settings.cfg.keys) {
            // Split "Mod+Mod+Key" into modifiers and the key.
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
            for (const auto& m : needMods)
                if (!mod_active(this, m)) { modsOk = false; break; }
            if (!modsOk) continue;

            wm.run_action(kb);
            return; // swallow the binding
        }
    }

    // Not a binding: forward to clients.
    wm.scene.handleKeyboardKeyEvent(event, Louvre::LEventOptionsDefault);
}

} // namespace lewm
