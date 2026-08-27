#ifndef LEWM_SETTINGS_HPP
#define LEWM_SETTINGS_HPP

#include "config/le_ast.hpp"
#include <string>

namespace lewm {

// Holds the live configuration. Loaded once at startup, then mutated at
// runtime by the settings panel or IPC. save() writes it back to config.le.
class Settings {
public:
    Config cfg;

    void load(const std::string& path);
    void save(const std::string& path) const;

    // Runtime setter used by `set <key> <value>` over IPC and the panel.
    // Key forms: gap, border_width, default_layout, panel.height,
    // anim.open_ms, anim.enabled, ...
    bool set(const std::string& key, const std::string& value);
};

} // namespace lewm

#endif
