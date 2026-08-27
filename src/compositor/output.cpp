#include "output.hpp"
#include "global.hpp"
#include "scene.hpp"

#include <Louvre/LAnimation.h>
#include <Louvre/LScreenshotRequest.h>

namespace lewm {

void LeWMOutput::initializeGL() {
    lewm::self().scene.handleInitializeGL(this);

    Louvre::LWeak<LeWMOutput> weak(this);
    fadeInView.insertAfter(&lewm::self().scene.layers[Louvre::LLayerOverlay]);
    fadeInView.setOpacity(0.f);

    Louvre::LAnimation::oneShot(1000,
        [weak](Louvre::LAnimation* anim) {
            if (!weak) { anim->stop(); return; }
            weak->fadeInView.setPos(weak->pos());
            weak->fadeInView.setSize(weak->size());
            weak->fadeInView.setOpacity(1.f - std::powf(anim->value(), 5.f));
            weak->repaint();
        },
        [weak](Louvre::LAnimation*) {
            if (weak) weak->fadeInView.setParent(nullptr);
        });
}

void LeWMOutput::paintGL() {
    lewm::self().scene.handlePaintGL(this);

    for (Louvre::LScreenshotRequest* req : screenshotRequests())
        req->accept(true);
}

void LeWMOutput::moveGL() {
    lewm::self().scene.handleMoveGL(this);
}

void LeWMOutput::resizeGL() {
    lewm::self().scene.handleResizeGL(this);
}

void LeWMOutput::uninitializeGL() {
    lewm::self().scene.handleUninitializeGL(this);
}

void LeWMOutput::setGammaRequest(Louvre::LClient* client, const Louvre::LGammaTable* gamma) {
    Louvre::L_UNUSED(client);
    setGamma(gamma);
}

void LeWMOutput::availableGeometryChanged() {
    LOutput::availableGeometryChanged();
}

} // namespace lewm
