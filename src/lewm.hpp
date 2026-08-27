#ifndef LEWM_LEWM_HPP
#define LEWM_LEWM_HPP

#include "config/le_ast.hpp"
#include "compositor/dwl_backend.hpp"
#include "layout/tiling.hpp"
#include <memory>

namespace le {

// Top-level compositor: owns config, backend, tiler and IPC.
class LeWM {
public:
    explicit LeWM(Config cfg);
    ~LeWM();

    void run();

private:
    Config cfg_;
    std::unique_ptr<DwlBackend> backend_;
    Tiler tiler_;
};

} // namespace le

#endif
