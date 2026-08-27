#ifndef LEWM_LE_AST_HPP
#define LEWM_LE_AST_HPP

#include <string>
#include <vector>
#include <map>

namespace le {

struct KeyBinding {
    std::string combo;
    std::string action;
    std::vector<std::string> args;
};

struct Workspace {
    std::string id;
    std::string name;
};

struct Rule {
    std::string app_id;
    std::string workspace;
    bool floating = false;
};

struct Config {
    std::string modkey = "Super";
    int border_width = 1;
    std::string border_active = "#3c3836";
    std::string border_normal = "#1d2021";
    std::string default_layout = "tile";
    bool xwayland = false;
    std::vector<KeyBinding> keys;
    std::vector<Workspace> workspaces;
    std::vector<Rule> rules;
};

} // namespace le

#endif
