#ifndef LEWM_ANIMATOR_HPP
#define LEWM_ANIMATOR_HPP

#include <Louvre/LAnimation.h>
#include <Louvre/LSurface.h>
#include <Louvre/LView.h>
#include <functional>
#include <string>

namespace lewm {

// Drives the "smooth" part of LeWM. Each window transition is a short
// tween over the compositor event loop using Louvre's LAnimation. We keep
// the math in easing.hpp so the curves are easy to swap from config.
class WindowAnimator {
public:
    // Fade a surface in. `view` is its scene view.
    void open(Louvre::LSurface* s, Louvre::LView* view,
              int ms, const std::string& easing);

    // Fade a surface out, then run done().
    void close(Louvre::LSurface* s, Louvre::LView* view,
               int ms, const std::string& easing, std::function<void()> done);

    // Slide a surface from one position to another.
    void move(Louvre::LSurface* s,
              const Louvre::LPoint& from, const Louvre::LPoint& to,
              int ms, const std::string& easing);
};

} // namespace lewm

#endif
