#ifndef LEWM_COMPOSITOR_HPP
#define LEWM_COMPOSITOR_HPP

#include <LCompositor.h>
#include <map>
#include <memory>
#include <string>

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
    Tiling tiling;
    std::unique_ptr<SettingsPanel> panel;

    LayoutKind layout_kind = LayoutKind::Tile;
    std::string current_workspace = "1";

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
    void switchWorkspace(const std::string& id);
    void handleCommand(const std::string& line);

    std::string config_path;
    std::unique_ptr<Ipc> ipc;

private:
    std::map<Louvre::LSurface*, std::string> tags_;
    std::vector<Louvre::LSurface*> workspaceWindows(Louvre::LOutput* out);
};

} // namespace lewm

#endif
