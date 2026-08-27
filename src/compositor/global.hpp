#ifndef LEWM_GLOBAL_HPP
#define LEWM_GLOBAL_HPP

#include "lewm_compositor.hpp"

namespace lewm {

// Lazy singleton accessor. The compositor sets g_self in its constructor so
// surfaces and roles can reach the scene/layout without passing pointers
// through Louvre's factory.
extern LeWMCompositor* g_self;

inline LeWMCompositor& self() { return *g_self; }

} // namespace lewm

#endif
