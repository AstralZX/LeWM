#ifndef LEWM_OUTPUT_HPP
#define LEWM_OUTPUT_HPP

#include <LOutput.h>
#include <LSolidColorView.h>

namespace lewm {

class LeWMOutput final : public Louvre::LOutput {
public:
    using LOutput::LOutput;

    void initializeGL() override;
    void paintGL() override;
    void moveGL() override;
    void resizeGL() override;
    void uninitializeGL() override;

    void setGammaRequest(Louvre::LClient* client, const Louvre::LGammaTable* gamma) override;
    void availableGeometryChanged() override;

    Louvre::LSolidColorView fadeInView { { 0.f, 0.f, 0.f } };
};

} // namespace lewm

#endif
