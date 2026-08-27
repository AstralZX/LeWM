#include "animator.hpp"
#include "easing.hpp"

#include <Louvre/LWeak.h>

namespace lewm {

namespace {
Louvre::LPoint lerp(const Louvre::LPoint& a, const Louvre::LPoint& b, float t) {
    return Louvre::LPoint(
        (Louvre::Int32)(a.x() + (b.x() - a.x()) * t),
        (Louvre::Int32)(a.y() + (b.y() - a.y()) * t));
}
} // namespace

void WindowAnimator::open(Louvre::LSurface* s, Louvre::LView* view,
                          int ms, const std::string& easing) {
    if (!s || !view) return;
    const std::string e = easing;
    view->setOpacity(0.f);
    Louvre::LWeak<Louvre::LSurface> w(s);
    Louvre::LAnimation::oneShot(ms,
        [w, view, e](Louvre::LAnimation* anim) {
            if (!w) { anim->stop(); return; }
            view->setOpacity(ease::resolve(e, anim->value()));
            w->repaintOutputs();
        });
}

void WindowAnimator::close(Louvre::LSurface* s, Louvre::LView* view,
                           int ms, const std::string& easing,
                           std::function<void()> done) {
    if (!s || !view) { if (done) done(); return; }
    const std::string e = easing;
    Louvre::LWeak<Louvre::LSurface> w(s);
    Louvre::LAnimation::oneShot(ms,
        [w, view, e](Louvre::LAnimation* anim) {
            if (!w) { anim->stop(); return; }
            view->setOpacity(1.f - ease::resolve(e, anim->value()));
            w->repaintOutputs();
        },
        [w, view, done](Louvre::LAnimation*) {
            if (view) view->setVisible(false);
            if (done) done();
        });
}

void WindowAnimator::move(Louvre::LSurface* s,
                          const Louvre::LPoint& from, const Louvre::LPoint& to,
                          int ms, const std::string& easing) {
    if (!s) return;
    const std::string e = easing;
    Louvre::LWeak<Louvre::LSurface> w(s);
    Louvre::LAnimation::oneShot(ms,
        [w, from, to, e](Louvre::LAnimation* anim) {
            if (!w) { anim->stop(); return; }
            w->setPos(lerp(from, to, ease::resolve(e, anim->value())));
            w->repaintOutputs();
        });
}

} // namespace lewm
