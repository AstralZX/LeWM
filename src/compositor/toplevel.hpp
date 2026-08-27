#ifndef LEWM_TOPLEVEL_HPP
#define LEWM_TOPLEVEL_HPP

#include <LToplevelRole.h>

namespace lewm {

// Standard desktop window. We let Louvre handle most of it and only set the
// bounds/capabilities we want; actual placement is done by the tiling layout
// in response to mapping changes.
class LeWMToplevelRole final : public Louvre::LToplevelRole {
public:
    using LToplevelRole::LToplevelRole;

    void configureRequest() override;
    void setMinimizedRequest() override;
};

} // namespace lewm

#endif
