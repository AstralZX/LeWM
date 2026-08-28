#ifndef LEWM_GLOBAL_HPP
#define LEWM_GLOBAL_HPP

#include "lewm_compositor.hpp"

namespace lewm {

extern LeWMCompositor* g_self;

inline LeWMCompositor& self() { return *g_self; }

} // namespace lewm

#endif
