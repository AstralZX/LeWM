#ifndef LEWM_TOPLEVEL_HPP
#define LEWM_TOPLEVEL_HPP

#include <LToplevelRole.h>

namespace lewm {

class LeWMToplevelRole final : public Louvre::LToplevelRole {
public:
    using LToplevelRole::LToplevelRole;

    void configureRequest() override;
    void setMinimizedRequest() override;
};

} // namespace lewm

#endif
