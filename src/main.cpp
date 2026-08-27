#pragma once

#include "lewm.hpp"
#include "config/le_parser.hpp"

#include <cstdlib>
#include <iostream>
#include <string>


namespace {

std::string config_path() {
    const char* xdg = std::getenv("XDG_CONFIG_HOME");
    std::string base = xdg ? std::string(xdg) : (std::getenv("HOME") + std::string("/.config"));
    return base + "/LeWM/config.le";
}

std::string runtime_dir() {
    const char* x = std::getenv("XDG_RUNTIME_DIR");
    return x ? std::string(x) : "/tmp";
}

} // namespace

int main(int argc, char** argv) {
    std::string path = config_path();
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-c" && i + 1 < argc) path = argv[++i];
        else if (a == "-h" || a == "--help") {
            std::cout << "LeWM [options]\n"
                         "  -c <file>   config.le path\n"
                         "  -h          this help\n";
            return 0;
        }
    }

    le::Config cfg;
    try {
        cfg = le::load_config(path);
    } catch (const std::exception& e) {
        std::cerr << "config error: " << e.what() << " (using defaults)\n";
    }

    try {
        le::LeWM wm(std::move(cfg));
        wm.run();
    } catch (const std::exception& e) {
        std::cerr << "fatal: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
