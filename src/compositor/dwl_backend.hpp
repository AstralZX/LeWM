#ifndef LEWM_DWL_BACKEND_HPP
#define LEWM_DWL_BACKEND_HPP

#include "le_ast.hpp"
#include <memory>

struct wl_display;
struct wlr_backend;

namespace le {

// Thin C++ wrapper around the dwl/wlroots core. Owns the Wayland display
// and the backend, and wires the parsed Config into the seat, output and
// xwayland setup.
class DwlBackend {
public:
    DwlBackend(const Config& cfg);
    ~DwlBackend();

    DwlBackend(const DwlBackend&) = delete;
    DwlBackend& operator=(const DwlBackend&) = delete;

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
