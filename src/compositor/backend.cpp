#include "backend.hpp"

#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_xwayland.h>

#include <stdexcept>

namespace le {

Compositor::Compositor(const Config& cfg) : cfg_(cfg) {
    display_ = wl_display_create();
    if (!display_) throw std::runtime_error("wl_display_create failed");

    backend_ = wlr_backend_autocreate(display_, nullptr);
    if (!backend_) throw std::runtime_error("wlr_backend_autocreate failed");

    setup_outputs();
    setup_seat();
    if (cfg_.xwayland) setup_xwayland();
}

Compositor::~Compositor() {
    if (backend_) wlr_backend_destroy(backend_);
    if (display_) wl_display_destroy(display_);
}

void Compositor::setup_outputs() {
    // Output layout is populated as outputs are advertised by the backend.
    // Placeholder: real code registers wlr_output_layout and adds listeners.
}

void Compositor::setup_seat() {
    // Placeholder: create the seat, attach keyboard/pointer/touch, and
    // forward input events to the tiler.
}

void Compositor::setup_xwayland() {
    // Placeholder: wlr_xwayland_create and lazy bind the X socket.
}

void Compositor::run() {
    running_ = true;
    if (!wl_display_run(display_)) running_ = false;
}

void Compositor::stop() {
    running_ = false;
    if (display_) wl_display_terminate(display_);
}

} // namespace le
