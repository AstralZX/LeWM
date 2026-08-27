#include "lewm.hpp"

namespace le {

LeWM::LeWM(Config cfg)
    : cfg_(std::move(cfg)),
      backend_(std::make_unique<DwlBackend>(cfg_)) {}

LeWM::~LeWM() = default;

void LeWM::run() {
    backend_->run();
}

} // namespace le
