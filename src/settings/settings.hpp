#ifndef LEWM_SETTINGS_HPP
#define LEWM_SETTINGS_HPP

#include "config/le_ast.hpp"
#include <string>

namespace lewm {

class Settings {
public:
    Config cfg;

    void load(const std::string& path);
    void save(const std::string& path) const;

    bool set(const std::string& key, const std::string& value);
};

} // namespace lewm

#endif
