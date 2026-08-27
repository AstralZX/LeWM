#include "lewm_compositor.hpp"
#include "global.hpp"
#include "output.hpp"
#include "surface.hpp"
#include "toplevel.hpp"
#include "keyboard.hpp"

#include <Louvre/LLauncher.h>
#include <Louvre/LLog.h>
#include <Louvre/LScene.h>
#include <cstdlib>

namespace lewm {

LeWMCompositor* g_self = nullptr;

LeWMCompositor::LeWMCompositor() : LCompositor() {
    g_self = this;

    const char* xdg = std::getenv("XDG_CONFIG_HOME");
    std::string base = xdg ? xdg : (std::getenv("HOME") + std::string("/.config"));
    config_path = base + "/LeWM/config.le";

    settings.load(config_path);
    panel = std::make_unique<SettingsPanel>(&scene.layers[Louvre::LLayerOverlay],
                                            hex_to_color(settings.cfg.panel.bg));
}

LeWMCompositor::~LeWMCompositor() {
    g_self = nullptr;
}

void LeWMCompositor::initialized() {
    setenv("WAYLAND_DISPLAY", std::getenv("LOUVRE_WAYLAND_DISPLAY"), 1);

    Louvre::Int32 totalWidth = 0;
    for (Louvre::LOutput* output : seat()->outputs()) {
        output->setScale(output->dpi() >= 200 ? 2.f : 1.f);
        output->setTransform(Louvre::LTransform::Normal);
        output->setPos(Louvre::LPoint(totalWidth, 0));
        totalWidth += output->size().w();
        addOutput(output);

        if (output == seat()->outputs().front())
            panel->setGeometry(output->availableGeometry());
    }

    for (const auto& app : settings.cfg.startup)
        Louvre::LLauncher::launch(app.cmd);

    const char* rd = std::getenv("XDG_RUNTIME_DIR");
    ipc = std::make_unique<Ipc>(rd ? rd : "/tmp",
                                [this](const std::string& line) { handleCommand(line); });
}

void LeWMCompositor::uninitialized() {
    ipc.reset();
    panel.reset();
}

Louvre::LFactoryObject* LeWMCompositor::createObjectRequest(
        Louvre::LFactoryObject::Type objectType, const void* params) {
    if (objectType == Louvre::LFactoryObject::Type::LOutput)
        return new LeWMOutput(params);
    if (objectType == Louvre::LFactoryObject::Type::LSurface)
        return new LeWMSurface(params);
    if (objectType == Louvre::LFactoryObject::Type::LToplevelRole)
        return new LeWMToplevelRole(params);
    if (objectType == Louvre::LFactoryObject::Type::LKeyboard)
        return new LeWMKeyboard(params);

    return nullptr;
}

bool LeWMCompositor::createGlobalsRequest() {
    return LCompositor::createGlobalsRequest();
}

bool LeWMCompositor::globalsFilter(Louvre::LClient*, Louvre::LGlobal*) {
    return true;
}

std::vector<Louvre::LSurface*> LeWMCompositor::workspaceWindows(Louvre::LOutput* out) {
    std::vector<Louvre::LSurface*> wins;
    for (Louvre::LSurface* s : surfaces()) {
        if (!s->mapped() || !s->toplevel()) continue;
        std::string t = tags_[s];
        if (t.empty()) t = current_workspace;
        if (t != current_workspace) continue;
        if (out->availableGeometry().contains(s->pos()))
            wins.push_back(s);
    }
    return wins;
}

void LeWMCompositor::relayout(Louvre::LOutput* output) {
    auto wins = workspaceWindows(output);
    tiling.relayout(layout_kind, output, wins, animator, settings.cfg);
}

void LeWMCompositor::tagSurface(Louvre::LSurface* s, const std::string& ws) {
    tags_[s] = ws;
    s->setMinimized(ws != current_workspace);
}

void LeWMCompositor::runAction(const KeyBinding& kb) {
    if (kb.action == "exec") {
        std::string cmd;
        for (size_t i = 0; i < kb.args.size(); ++i) {
            if (i) cmd += ' ';
            cmd += kb.args[i];
        }
        Louvre::LLauncher::launch(cmd);
    } else if (kb.action == "layout_next") {
        cycleLayout();
    } else if (kb.action == "kill_focused") {
        killFocused();
    } else if (kb.action == "focus_next") {
        cycleFocus(1);
    } else if (kb.action == "focus_prev") {
        cycleFocus(-1);
    } else if (kb.action == "workspace" && !kb.args.empty()) {
        switchWorkspace(kb.args[0]);
    } else if (kb.action == "toggle_panel") {
        panel->toggle();
    } else if (kb.action == "relayout") {
        for (Louvre::LOutput* o : outputs()) relayout(o);
    }
}

void LeWMCompositor::killFocused() {
    Louvre::LSurface* s = seat()->keyboard()->focus();
    if (s && s->toplevel())
        s->toplevel()->close();
}

void LeWMCompositor::cycleFocus(int dir) {
    std::vector<Louvre::LSurface*> all;
    for (Louvre::LOutput* o : outputs())
        for (Louvre::LSurface* s : workspaceWindows(o))
            all.push_back(s);
    if (all.empty()) return;

    Louvre::LSurface* focused = seat()->keyboard()->focus();
    int idx = 0;
    for (size_t i = 0; i < all.size(); ++i)
        if (all[i] == focused) idx = (int)i;

    idx = (idx + dir + (int)all.size()) % (int)all.size();
    seat()->keyboard()->setFocus(all[idx]);
}

void LeWMCompositor::cycleLayout() {
    layout_kind = (layout_kind == LayoutKind::Tile) ? LayoutKind::Grid : LayoutKind::Tile;
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::switchWorkspace(const std::string& id) {
    current_workspace = id;
    for (Louvre::LSurface* s : surfaces()) {
        if (!s->toplevel()) continue;
        std::string t = tags_[s];
        if (t.empty()) t = id;
        s->setMinimized(t != id);
    }
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::handleCommand(const std::string& line) {
    std::istringstream is(line);
    std::string cmd;
    is >> cmd;

    if (cmd == "toggle_panel") {
        panel->toggle();
    } else if (cmd == "relayout") {
        for (Louvre::LOutput* o : outputs()) relayout(o);
    } else if (cmd == "layout_next") {
        cycleLayout();
    } else if (cmd == "switch" && !line.empty()) {
        std::string id;
        is >> id;
        if (!id.empty()) switchWorkspace(id);
    } else if (cmd == "set") {
        std::string key, val;
        is >> key >> val;
        if (settings.set(key, val))
            settings.save(config_path);
    }
}

} // namespace lewm
