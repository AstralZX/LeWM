#ifndef LEWM_EASING_HPP
#define LEWM_EASING_HPP

#include <string>

namespace lewm {
namespace ease {

float linear(float t);
float easeOutQuad(float t);
float easeOutCubic(float t);
float easeInOutCubic(float t);
float easeOutBack(float t);

// Resolves a name from config (e.g. "easeOutCubic") to a function.
float resolve(const std::string& name, float t);

} // namespace ease
} // namespace lewm

#endif
