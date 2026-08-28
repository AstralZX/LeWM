#ifndef LEWM_ANIMATOR_HPP
#define LEWM_ANIMATOR_HPP

#include <Louvre/LAnimation.h>
#include <Louvre/LSurface.h>
#include <Louvre/LView.h>
#include <functional>
#include <string>

namespace lewm {

class WindowAnimator {
public:
    void open(Louvre::LSurface* s, Louvre::LView* view,
              int ms, const std::string& easing);

    void close(Louvre::LSurface* s, Louvre::LView* view,
               int ms, const std::string& easing, std::function<void()> done);

    void move(Louvre::LSurface* s,
              const Louvre::LPoint& from, const Louvre::LPoint& to,
              int ms, const std::string& easing);
};

} // namespace lewm

#endif
