#include <Louvre/LLauncher.h>
#include <Louvre/LLog.h>
#include <unistd.h>

#include "compositor/lewm_compositor.hpp"

int main(int, char*[]) {
    setenv("LOUVRE_DEBUG", "1", 0);
    setenv("MOZ_ENABLE_WAYLAND", "1", 1);
    setenv("QT_QPA_PLATFORM", "wayland-egl", 1);
    setenv("GDK_BACKEND", "wayland", 1);
    setenv("LOUVRE_WAYLAND_DISPLAY", "wayland-2", 0);

    Louvre::LLauncher::startDaemon();

    lewm::LeWMCompositor compositor;

    if (!compositor.start()) {
        Louvre::LLog::fatal("Failed to start LeWM.");
        return EXIT_FAILURE;
    }

    while (compositor.state() != Louvre::LCompositor::Uninitialized)
        compositor.processLoop(-1);

    return EXIT_SUCCESS;
}
