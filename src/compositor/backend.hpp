#ifndef LEWM_BACKEND_HPP
#define LEWM_BACKEND_HPP

#include "config/le_ast.hpp"
#include <memory>

struct wl_display;
struct wlr_backend;

namespace le {

// LeWM's compositor core, built directly on wlroots. Owns the Wayland
// display and the backend, and wires the parsed Config into the seat,
// output and xwayland setup.
class Compositor {
public:
    Compositor(const Config& cfg);
    ~Compositor();

    Compositor(const Compositor&) = delete;
    Compositor& operator=(const Compositor&) = delete;

    void run();
    void stop();

    wl_display* display() const { return display_; }

private:
    void setup_outputs();
    void setup_seat();
    void setup_xwayland();

    Config cfg_;
    wl_display* display_ = nullptr;
    wlr_backend* backend_ = nullptr;
    bool running_ = false;
};

} // namespace le

#endif
