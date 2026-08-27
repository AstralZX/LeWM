#ifndef LEWM_COMPOSITOR_HPP
#define LEWM_COMPOSITOR_HPP

#include <LCompositor.h>
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

protected:
    void initialized() override;
    void uninitialized() override;

    Louvre::LFactoryObject* createObjectRequest(Louvre::LFactoryObject::Type objectType,
                                                const void* params) override;
    bool createGlobalsRequest() override;
    bool globalsFilter(Louvre::LClient* client, Louvre::LGlobal* global) override;

public:
    // Re-tiles every mapped toplevel on the given output.
    void relayout(Louvre::LOutput* output);

    // Driven by the IPC socket.
    void handle_command(const std::string& line);

    std::string config_path;
    std::unique_ptr<Ipc> ipc;
};

} // namespace lewm

#endif
