#include "lewm_compositor.hpp"
#include "global.hpp"
#include "output.hpp"
#include "surface.hpp"
#include "toplevel.hpp"

#include <Louvre/LLauncher.h>
#include <Louvre/LLog.h>
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
                                [this](const std::string& line) { handle_command(line); });
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

    // Everything else uses Louvre's default implementation for now.
    return nullptr;
}

bool LeWMCompositor::createGlobalsRequest() {
    return LCompositor::createGlobalsRequest();
}

bool LeWMCompositor::globalsFilter(Louvre::LClient*, Louvre::LGlobal*) {
    return true;
}

void LeWMCompositor::relayout(Louvre::LOutput* output) {
    std::vector<Louvre::LSurface*> wins;
    for (Louvre::LSurface* s : surfaces()) {
        if (!s->mapped() || !s->toplevel()) continue;
        if (output->availableGeometry().contains(s->pos()))
            wins.push_back(s);
    }
    tiling.relayout(output, wins, animator, settings.cfg);
}

void LeWMCompositor::handle_command(const std::string& line) {
    std::istringstream is(line);
    std::string cmd;
    is >> cmd;

    if (cmd == "toggle_panel") {
        panel->toggle();
    } else if (cmd == "set") {
        std::string key, val;
        is >> key >> val;
        if (settings.set(key, val))
            settings.save(config_path);
    } else if (cmd == "relayout") {
        for (Louvre::LOutput* o : outputs()) relayout(o);
    }
    // layout_next / kill_focused / focus_next are wired to the seat later.
}

} // namespace lewm
