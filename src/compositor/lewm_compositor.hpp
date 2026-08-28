#pragma once

#include <LCompositor.h>
#include <Louvre/LTimer.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "scene.hpp"
#include "config/le_ast.hpp"
#include "settings/settings.hpp"
#include "settings/panel.hpp"
#include "anim/animator.hpp"
#include "layout/tiling.hpp"
#include "ipc/ipc.hpp"

namespace lewm {

class LeWMCompositor final : public Louvre::LCompositor {
public:
    LeWMCompositor();
    ~LeWMCompositor();

    LeWMScene scene;
    Settings settings;
    WindowAnimator animator;
    std::unique_ptr<SettingsPanel> panel;

    Layout layout_kind = Layout::Tile;
    float split_ratio = 0.6f;
    std::string current_workspace = "1";

    bool resize_mode = false;

protected:
    void initialized() override;
    void uninitialized() override;

    Louvre::LFactoryObject* createObjectRequest(Louvre::LFactoryObject::Type objectType,
                                                const void* params) override;
    bool createGlobalsRequest() override;
    bool globalsFilter(Louvre::LClient* client, Louvre::LGlobal* global) override;

public:
    void relayout(Louvre::LOutput* output);
    void tagSurface(Louvre::LSurface* s, const std::string& ws);
    void runAction(const KeyBinding& kb);
    void killFocused();
    void cycleFocus(int dir);
    void cycleLayout();
    void switchLayout(const std::string& id);
    void switchWorkspace(const std::string& id);
    void workspaceNext(int dir);
    void focusOutput(int dir);
    void focusLast();
    void handleCommand(const std::string& line);

    void toggleGaps();
    void setRatio(float r);
    void setOutputGap(Louvre::LOutput* out, int gap);
    void reloadConfig();

    void toggleFullscreen(Louvre::LSurface* s);
    void toggleFloat(Louvre::LSurface* s);
    void setFloating(Louvre::LSurface* s, bool on);
    void setFullscreen(Louvre::LSurface* s, bool on);
    void setSticky(Louvre::LSurface* s, bool on);
    void toggleSticky(Louvre::LSurface* s);
    void swapFocused();
    void setResizeMode(bool on);
    void nudgeResize(int dx, int dy);
    void onSurfaceUnmapped(Louvre::LSurface* s);
    void onSurfaceMapped(Louvre::LSurface* s);
    void updateFocus(Louvre::LSurface* s);
    void focusSurface(Louvre::LSurface* s);

    bool isFloating(Louvre::LSurface* s) const { return floating_.count(s) != 0; }
    bool isFullscreen(Louvre::LSurface* s) const { return fullscreen_.count(s) != 0; }

    std::string config_path;
    std::unique_ptr<Ipc> ipc;
    std::unique_ptr<Louvre::LTimer> ipc_timer_;

private:
    std::map<Louvre::LSurface*, std::string> tags_;
    std::map<std::string, Layout> ws_layout_;
    std::map<std::string, float> ws_ratio_;
    std::map<std::string, int> out_gap_;
    std::map<std::string, std::vector<Louvre::LSurface*>> ws_order_;
    std::set<Louvre::LSurface*> floating_;
    std::set<Louvre::LSurface*> fullscreen_;
    std::set<Louvre::LSurface*> sticky_;
    std::vector<Louvre::LSurface*> mru_;
    bool gaps_enabled_ = true;

    Layout currentLayout();
    bool workspaceHasWindows(const std::string& id);
    float workspaceRatio() const;
    int workspaceGap(Louvre::LOutput* out) const;
    std::vector<Louvre::LSurface*> visibleWindows(Louvre::LOutput* out);
    std::vector<Louvre::LSurface*> tileWindows(Louvre::LOutput* out);
};

} // namespace lewm
