#include "easing.hpp"
#include <cmath>

namespace lewm {
namespace ease {

float linear(float t) { return t; }

float easeOutQuad(float t) { return 1.f - (1.f - t) * (1.f - t); }

float easeOutCubic(float t) { return 1.f - std::pow(1.f - t, 3.f); }

float easeInOutCubic(float t) {
    return t < 0.5f ? 4.f * t * t * t : 1.f - std::pow(-2.f * t + 2.f, 3.f) / 2.f;
}

float easeOutBack(float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.f;
    return 1.f + c3 * std::pow(t - 1.f, 3.f) + c1 * std::pow(t - 1.f, 2.f);
}

float resolve(const std::string& name, float t) {
    if (name == "linear")       return linear(t);
    if (name == "easeOutQuad")  return easeOutQuad(t);
    if (name == "easeInOutCubic") return easeInOutCubic(t);
    if (name == "easeOutBack")  return easeOutBack(t);
    return easeOutCubic(t);
}

} // namespace ease
} // namespace lewm
