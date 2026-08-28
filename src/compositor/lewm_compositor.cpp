#include "lewm_compositor.hpp"
#include "global.hpp"
#include "output.hpp"
#include "surface.hpp"
#include "toplevel.hpp"
#include "keyboard.hpp"
#include "pointer.hpp"
#include "settings/panel.hpp"

#include <Louvre/LLauncher.h>
#include <Louvre/LLog.h>
#include <Louvre/LScene.h>
#include <Louvre/LCursor.h>
#include <algorithm>
#include <cstdlib>

namespace lewm {

LeWMCompositor* g_self = nullptr;

LeWMCompositor::LeWMCompositor() : LCompositor() {
    g_self = this;

    const char* xdg = std::getenv("XDG_CONFIG_HOME");
    std::string base = xdg ? xdg : (std::getenv("HOME") + std::string("/.config"));
    config_path = base + "/LeWM/config.le";

    settings.load(config_path);
    layout_kind = layout_from_name(settings.cfg.default_layout);
    panel = std::make_unique<SettingsPanel>(&scene.layers[LayerOverlay],
                                            hex_to_color(settings.cfg.panel.bg));
}

LeWMCompositor::~LeWMCompositor() {
    g_self = nullptr;
}

void LeWMCompositor::initialized() {
    setenv("WAYLAND_DISPLAY", std::getenv("LOUVRE_WAYLAND_DISPLAY"), 1);

    for (const auto& w : settings.cfg.workspaces)
        ws_layout_[w.id] = layout_kind;

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
    if (objectType == Louvre::LFactoryObject::Type::LPointer)
        return new LeWMPointer(params);

    return nullptr;
}

bool LeWMCompositor::createGlobalsRequest() {
    return LCompositor::createGlobalsRequest();
}

bool LeWMCompositor::globalsFilter(Louvre::LClient*, Louvre::LGlobal*) {
    return true;
}

Layout LeWMCompositor::currentLayout() {
    auto it = ws_layout_.find(current_workspace);
    return it == ws_layout_.end() ? layout_kind : it->second;
}

bool LeWMCompositor::workspaceHasWindows(const std::string& id) {
    for (Louvre::LSurface* s : surfaces()) {
        if (!s->mapped() || !s->toplevel()) continue;
        if (sticky_.count(s)) continue;
        std::string t = tags_[s];
        if (t.empty()) t = id;
        if (t != id) continue;
        return true;
    }
    return false;
}

std::vector<Louvre::LSurface*> LeWMCompositor::visibleWindows(Louvre::LOutput* out) {
    std::vector<Louvre::LSurface*> wins;
    for (Louvre::LSurface* s : surfaces()) {
        if (!s->mapped() || !s->toplevel()) continue;
        bool sticky = sticky_.count(s);
        std::string t = tags_[s];
        if (t.empty()) t = current_workspace;
        if (!sticky && t != current_workspace) continue;
        if (out->availableGeometry().containsPoint(s->pos()))
            wins.push_back(s);
    }
    return wins;
}

std::vector<Louvre::LSurface*> LeWMCompositor::tileWindows(Louvre::LOutput* out) {
    const auto& order = ws_order_[current_workspace];
    const std::string cur = current_workspace;
    std::vector<Louvre::LSurface*> wins;
    for (Louvre::LSurface* s : order) {
        if (!s || !s->mapped() || !s->toplevel()) continue;
        if (sticky_.count(s) || floating_.count(s) || fullscreen_.count(s)) continue;
        if (out->availableGeometry().containsPoint(s->pos()))
            wins.push_back(s);
    }
    return wins;
}

void LeWMCompositor::relayout(Louvre::LOutput* output) {
    for (Louvre::LSurface* s : visibleWindows(output)) {
        if (!fullscreen_.count(s)) continue;
        s->setPos(output->pos());
        if (s->toplevel())
            s->toplevel()->configureSize(output->size().w(), output->size().h());
        s->raise();
    }
    auto wins = tileWindows(output);
    lewm::relayout(currentLayout(), output, wins, animator, settings.cfg,
                   workspaceRatio(), workspaceGap(output));
    for (Louvre::LSurface* s : visibleWindows(output)) {
        if (auto* ws = static_cast<LeWMSurface*>(s))
            ws->updateBorder();
    }
}

float LeWMCompositor::workspaceRatio() const {
    auto it = ws_ratio_.find(current_workspace);
    return it == ws_ratio_.end() ? split_ratio : it->second;
}

int LeWMCompositor::workspaceGap(Louvre::LOutput* out) const {
    if (!gaps_enabled_) return 0;
    if (out) {
        auto it = out_gap_.find(out->name());
        if (it != out_gap_.end()) return it->second;
    }
    return settings.cfg.gap;
}

void LeWMCompositor::tagSurface(Louvre::LSurface* s, const std::string& ws) {
    tags_[s] = ws;
    s->setMinimized(sticky_.count(s) ? false : (ws != current_workspace));
    auto& order = ws_order_[ws];
    if (std::find(order.begin(), order.end(), s) == order.end())
        order.push_back(s);
}

void LeWMCompositor::onSurfaceMapped(Louvre::LSurface* s) {
    if (!s || !s->toplevel()) return;
    std::string ws = current_workspace;
    std::string aid = s->toplevel()->appId();
    for (const auto& r : settings.cfg.rules) {
        if (r.app_id != aid) continue;
        if (!r.workspace.empty()) ws = r.workspace;
        if (r.fullscreen) setFullscreen(s, true);
        if (r.floating) setFloating(s, true);
        if (r.has_layout)
            ws_layout_[ws] = layout_from_name(r.layout);
        break;
    }
    if (!sticky_.count(s))
        tagSurface(s, ws);
    if (!isFloating(s) && s->parent())
        setFloating(s, true);
    for (Louvre::LOutput* o : outputs())
        relayout(o);
}

void LeWMCompositor::onSurfaceUnmapped(Louvre::LSurface* s) {
    for (auto& kv : ws_order_) {
        auto& v = kv.second;
        v.erase(std::remove(v.begin(), v.end(), s), v.end());
    }
    floating_.erase(s);
    fullscreen_.erase(s);
    sticky_.erase(s);
    mru_.erase(std::remove(mru_.begin(), mru_.end(), s), mru_.end());
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
    } else if (kb.action == "focus_last") {
        focusLast();
    } else if (kb.action == "swap_focused") {
        swapFocused();
    } else if (kb.action == "workspace" && !kb.args.empty()) {
        switchWorkspace(kb.args[0]);
    } else if (kb.action == "workspace_next") {
        workspaceNext(1);
    } else if (kb.action == "workspace_prev") {
        workspaceNext(-1);
    } else if (kb.action == "toggle_panel") {
        panel->toggle();
    } else if (kb.action == "relayout") {
        for (Louvre::LOutput* o : outputs()) relayout(o);
    } else if (kb.action == "fullscreen") {
        Louvre::LSurface* s = seat()->keyboard()->focus();
        if (s) toggleFullscreen(s);
    } else if (kb.action == "float") {
        Louvre::LSurface* s = seat()->keyboard()->focus();
        if (s) toggleFloat(s);
    } else if (kb.action == "sticky") {
        Louvre::LSurface* s = seat()->keyboard()->focus();
        if (s) toggleSticky(s);
    } else if (kb.action == "focus_output_next") {
        focusOutput(1);
    } else if (kb.action == "focus_output_prev") {
        focusOutput(-1);
    } else if (kb.action == "layout" && !kb.args.empty()) {
        switchLayout(kb.args[0]);
    } else if (kb.action == "resize_grow") {
        nudgeResize(1, 0);
    } else if (kb.action == "resize_shrink") {
        nudgeResize(-1, 0);
    } else if (kb.action == "gaps_toggle") {
        toggleGaps();
    } else if (kb.action == "ratio_default") {
        setRatio(0.5f);
    } else if (kb.action == "ratio_wide") {
        setRatio(0.65f);
    } else if (kb.action == "ratio_narrow") {
        setRatio(0.35f);
    }
}

void LeWMCompositor::switchLayout(const std::string& name) {
    ws_layout_[current_workspace] = layout_from_name(name);
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::killFocused() {
    Louvre::LSurface* s = seat()->keyboard()->focus();
    if (s && s->toplevel())
        s->toplevel()->close();
}

void LeWMCompositor::focusSurface(Louvre::LSurface* s) {
    if (!s) return;
    Louvre::LSurface* prev = seat()->keyboard()->focus();
    seat()->keyboard()->setFocus(s);
    mru_.erase(std::remove(mru_.begin(), mru_.end(), s), mru_.end());
    mru_.push_back(s);
    if (auto* p = static_cast<LeWMSurface*>(prev)) p->updateBorder();
    if (auto* n = static_cast<LeWMSurface*>(s)) n->updateBorder();
}

void LeWMCompositor::updateFocus(Louvre::LSurface* s) {
    if (!s) return;
    mru_.erase(std::remove(mru_.begin(), mru_.end(), s), mru_.end());
    mru_.push_back(s);
}

void LeWMCompositor::focusLast() {
    if (mru_.empty()) return;
    Louvre::LSurface* cur = seat()->keyboard()->focus();
    if (mru_.size() >= 2) {
        Louvre::LSurface* last = mru_[mru_.size() - 2];
        if (last && last->mapped()) {
            focusSurface(last);
            return;
        }
    }
    (void)cur;
}

void LeWMCompositor::cycleFocus(int dir) {
    std::vector<Louvre::LSurface*> all;
    for (Louvre::LOutput* o : outputs())
        for (Louvre::LSurface* s : tileWindows(o))
            if (std::find(all.begin(), all.end(), s) == all.end())
                all.push_back(s);
    if (all.empty()) return;

    Louvre::LSurface* focused = seat()->keyboard()->focus();
    int idx = 0;
    for (size_t i = 0; i < all.size(); ++i)
        if (all[i] == focused) idx = (int)i;

    idx = (idx + dir + (int)all.size()) % (int)all.size();
    focusSurface(all[idx]);
}

void LeWMCompositor::cycleLayout() {
    static const Layout order[] = { Layout::Tile, Layout::Grid,
                                    Layout::Dwindle, Layout::Master, Layout::Custom };
    Layout cur = currentLayout();
    int idx = 0;
    for (int i = 0; i < 5; ++i)
        if (order[i] == cur) idx = i;
    Layout next = order[(idx + 1) % 5];
    ws_layout_[current_workspace] = next;
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::switchWorkspace(const std::string& id) {
    current_workspace = id;
    if (!ws_layout_.count(id)) ws_layout_[id] = layout_kind;
    for (Louvre::LSurface* s : surfaces()) {
        if (!s->toplevel()) continue;
        if (sticky_.count(s)) {
            s->setMinimized(false);
            continue;
        }
        std::string t = tags_[s];
        if (t.empty()) t = id;
        s->setMinimized(t != id);
    }
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::workspaceNext(int dir) {
    std::vector<std::string> ids;
    for (const auto& w : settings.cfg.workspaces)
        ids.push_back(w.id);
    if (ids.empty())
        for (int i = 1; i <= 9; ++i) ids.push_back(std::to_string(i));

    int idx = -1;
    for (size_t i = 0; i < ids.size(); ++i)
        if (ids[i] == current_workspace) { idx = (int)i; break; }

    int n = (int)ids.size();
    for (int step = 1; step <= n; ++step) {
        int ni = (idx + dir * step + n) % n;
        if (ids[ni] == current_workspace) continue;
        if (workspaceHasWindows(ids[ni]) || step == n) {
            switchWorkspace(ids[ni]);
            return;
        }
    }
}

void LeWMCompositor::focusOutput(int dir) {
    Louvre::LOutput* cur = Louvre::cursor()->output();
    const auto& outs = seat()->outputs();
    if (outs.empty()) return;
    int idx = 0;
    for (size_t i = 0; i < outs.size(); ++i)
        if (outs[i] == cur) idx = (int)i;
    idx = (idx + dir + (int)outs.size()) % (int)outs.size();
    auto wins = visibleWindows(outs[idx]);
    if (!wins.empty())
        focusSurface(wins.front());
}

void LeWMCompositor::toggleFullscreen(Louvre::LSurface* s) {
    setFullscreen(s, !isFullscreen(s));
}

void LeWMCompositor::setFullscreen(Louvre::LSurface* s, bool on) {
    if (on) fullscreen_.insert(s);
    else fullscreen_.erase(s);
    if (s->toplevel()) {
        auto st = s->toplevel()->state();
        st.setFlag(Louvre::LToplevelRole::Fullscreen, on);
        s->toplevel()->configureState(st);
    }
    s->setMinimized(false);
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::toggleFloat(Louvre::LSurface* s) {
    setFloating(s, !isFloating(s));
}

void LeWMCompositor::setFloating(Louvre::LSurface* s, bool on) {
    if (on) {
        floating_.insert(s);
        if (s->toplevel() && s->pos() == Louvre::LPoint()) {
            Louvre::LOutput* out = Louvre::cursor()->output();
            if (out) {
                int w = s->size().w(), h = s->size().h();
                s->setPos({ out->pos().x() + (out->size().w() - w) / 2,
                            out->pos().y() + (out->size().h() - h) / 2 });
            }
        }
    } else {
        floating_.erase(s);
    }
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::setSticky(Louvre::LSurface* s, bool on) {
    if (on) sticky_.insert(s);
    else sticky_.erase(s);
    s->setMinimized(false);
}

void LeWMCompositor::toggleSticky(Louvre::LSurface* s) {
    if (!s) return;
    setSticky(s, !sticky_.count(s));
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::swapFocused() {
    Louvre::LSurface* f = seat()->keyboard()->focus();
    if (!f) return;
    auto& order = ws_order_[current_workspace];
    auto it = std::find(order.begin(), order.end(), f);
    if (it == order.end() || it + 1 == order.end()) return;
    std::iter_swap(it, it + 1);
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::setResizeMode(bool on) {
    resize_mode = on;
}

void LeWMCompositor::nudgeResize(int dx, int dy) {
    (void)dy;
    float r = workspaceRatio();
    r = std::clamp(r + 0.05f * dx, 0.2f, 0.8f);
    ws_ratio_[current_workspace] = r;
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::setRatio(float r) {
    ws_ratio_[current_workspace] = std::clamp(r, 0.2f, 0.8f);
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::toggleGaps() {
    gaps_enabled_ = !gaps_enabled_;
    for (Louvre::LOutput* o : outputs()) relayout(o);
}

void LeWMCompositor::setOutputGap(Louvre::LOutput* out, int gap) {
    if (!out) return;
    out_gap_[out->name()] = gap;
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
    } else if (cmd == "layout" || cmd == "set_layout") {
        std::string name;
        is >> name;
        if (!name.empty()) switchLayout(name);
    } else if (cmd == "switch" && !line.empty()) {
        std::string id;
        is >> id;
        if (!id.empty()) switchWorkspace(id);
    } else if (cmd == "set") {
        std::string key, val;
        is >> key >> val;
        if (settings.set(key, val))
            settings.save(config_path);
    } else if (cmd == "reload") {
        reloadConfig();
    } else if (cmd == "gaps") {
        toggleGaps();
    } else if (cmd == "ratio") {
        std::string v;
        is >> v;
        if (v == "default") setRatio(0.5f);
        else if (v == "wide") setRatio(0.65f);
        else if (v == "narrow") setRatio(0.35f);
    } else if (cmd == "gap") {
        int g;
        if (is >> g) setOutputGap(Louvre::cursor()->output(), g);
    }
}

void LeWMCompositor::reloadConfig() {
    settings.load(config_path);
    ws_layout_.clear();
    ws_ratio_.clear();
    for (const auto& w : settings.cfg.workspaces)
        ws_layout_[w.id] = layout_kind;
    for (Louvre::LSurface* s : surfaces()) {
        if (auto* ws = static_cast<LeWMSurface*>(s))
            ws->updateBorder();
    }
    for (Louvre::LOutput* o : outputs())
        relayout(o);
}

} // namespace lewm
