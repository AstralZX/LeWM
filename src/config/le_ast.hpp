#pragma once

#include <string>
#include <vector>

namespace lewm {

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
    bool fullscreen = false;
    bool has_layout = false;
    std::string layout;
};

struct StartupApp {
    std::string cmd;
};

struct AnimConfig {
    bool enabled = true;
    int open_ms = 220;
    int close_ms = 160;
    int move_ms = 130;
    std::string easing = "easeOutCubic";
};

struct PanelConfig {
    bool enabled = true;
    std::string position = "top";
    int height = 28;
    std::string bg = "#1d2021";
    std::string fg = "#ebdbb2";
};

struct Config {
    std::string modkey = "Super";
    int border_width = 2;
    std::string border_active = "#d65d0e";
    std::string border_normal = "#282828";
    std::string border_urgent = "#cc241d";
    int gap = 6;
    bool xwayland = false;
    std::string default_layout = "tile";

    AnimConfig anim;
    PanelConfig panel;

    std::vector<StartupApp> startup;
    std::vector<KeyBinding> keys;
    std::vector<Workspace> workspaces;
    std::vector<Rule> rules;
};

} // namespace lewm


