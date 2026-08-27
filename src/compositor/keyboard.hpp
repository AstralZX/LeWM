#ifndef LEWM_KEYBOARD_HPP
#define LEWM_KEYBOARD_HPP

#include <LKeyboard.h>

namespace lewm {

class LeWMKeyboard final : public Louvre::LKeyboard {
public:
    using LKeyboard::LKeyboard;

    void keyEvent(const Louvre::LKeyboardKeyEvent& event) override;
};

} // namespace lewm

#endif
