#include "le_parser.hpp"
#include "le_ast.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace le {

namespace {

std::string trim(const std::string& s) {
    std::size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return {};
    std::size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

std::vector<std::string> split_ws(const std::string& s) {
    std::vector<std::string> out;
    std::istringstream is(s);
    std::string tok;
    while (is >> tok) out.push_back(tok);
    return out;
}

} // namespace

Parser::Parser(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("cannot open config: " + path);
    std::stringstream ss;
    ss << f.rdbuf();
    source_ = ss.str();
    std::string line;
    while (std::getline(ss, line)) lines_.push_back(line);
}

Config Parser::parse() {
    Config cfg;
    for (const auto& raw : lines_) {
        std::string line = trim(raw);
        if (line.empty() || line.starts_with("#")) continue;

        auto toks = split_ws(line);
        if (toks.empty()) continue;
        const std::string& cmd = toks[0];

        if (cmd == "modkey" && toks.size() >= 2) {
            cfg.modkey = toks[1];
        } else if (cmd == "border_width" && toks.size() >= 2) {
            cfg.border_width = std::stoi(toks[1]);
        } else if (cmd == "border_color_active" && toks.size() >= 2) {
            cfg.border_active = toks[1];
        } else if (cmd == "border_color_normal" && toks.size() >= 2) {
            cfg.border_normal = toks[1];
        } else if (cmd == "layout" && toks.size() >= 3 && toks[1] == "default") {
            cfg.default_layout = toks[2];
        } else if (cmd == "xwayland" && toks.size() >= 2) {
            cfg.xwayland = (toks[1] == "true");
        } else if (cmd == "key" && toks.size() >= 3) {
            // key "Mod+Enter" exec "foot"
            KeyBinding k;
            k.combo = toks[1];
            k.action = toks[2];
            for (std::size_t i = 3; i < toks.size(); ++i) k.args.push_back(toks[i]);
            cfg.keys.push_back(k);
        } else if (cmd == "workspace" && toks.size() >= 3) {
            // workspace "1" "term"
            Workspace w;
            w.id = toks[1];
            w.name = toks[2];
            cfg.workspaces.push_back(w);
        } else if (cmd == "rule" && toks.size() >= 3) {
            // rule "firefox" workspace "2" float false
            Rule r;
            r.app_id = toks[1];
            for (std::size_t i = 2; i < toks.size(); ++i) {
                if (toks[i] == "workspace" && i + 1 < toks.size()) {
                    r.workspace = toks[++i];
                } else if (toks[i] == "float" && i + 1 < toks.size()) {
                    r.floating = (toks[++i] == "true");
                }
            }
            cfg.rules.push_back(r);
        }
    }
    return cfg;
}

Config load_config(const std::string& path) {
    Parser p(path);
    return p.parse();
}

} // namespace le
