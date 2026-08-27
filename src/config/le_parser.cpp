#include "le_parser.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace lewm {

namespace {

std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return {};
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

std::vector<std::string> split_ws(const std::string& s) {
    std::vector<std::string> out;
    std::istringstream is(s);
    std::string t;
    while (is >> t) out.push_back(t);
    return out;
}

// Pull a quoted token starting at index i, returns the unquoted text and
// advances i past the closing quote.
std::string take_quoted(const std::vector<std::string>& toks, size_t& i) {
    std::string raw = toks[i];
    std::string out;
    bool open = false;
    for (size_t j = i; j < toks.size(); ++j) {
        std::string piece = toks[j];
        if (!open && !piece.empty() && piece.front() == '"') {
            open = true;
            piece = piece.substr(1);
        }
        if (!out.empty()) out += ' ';
        out += piece;
        if (!piece.empty() && piece.back() == '"') {
            out.pop_back();
            i = j;
            return out;
        }
    }
    i = toks.size() - 1;
    return out;
}

} // namespace

Config Parser::parse() {
    std::ifstream f(path_);
    if (!f) throw std::runtime_error("cannot open config: " + path_);

    Config cfg;
    std::string raw;
    while (std::getline(f, raw)) {
        std::string line = trim(raw);
        if (line.empty() || line.starts_with("#")) continue;

        auto toks = split_ws(line);
        if (toks.empty()) continue;
        const std::string cmd = toks[0];

        if (cmd == "modkey" && toks.size() >= 2) {
            cfg.modkey = toks[1];
        } else if (cmd == "border_width" && toks.size() >= 2) {
            cfg.border_width = std::stoi(toks[1]);
        } else if (cmd == "border_color_active" && toks.size() >= 2) {
            cfg.border_active = toks[1];
        } else if (cmd == "border_color_normal" && toks.size() >= 2) {
            cfg.border_normal = toks[1];
        } else if (cmd == "border_color_urgent" && toks.size() >= 2) {
            cfg.border_urgent = toks[1];
        } else if (cmd == "gap" && toks.size() >= 2) {
            cfg.gap = std::stoi(toks[1]);
        } else if (cmd == "xwayland" && toks.size() >= 2) {
            cfg.xwayland = (toks[1] == "true");
        } else if (cmd == "layout" && toks.size() >= 3 && toks[1] == "default") {
            cfg.default_layout = toks[2];
        } else if (cmd == "animation" && toks.size() >= 3) {
            if (toks[1] == "enabled") cfg.anim.enabled = (toks[2] == "true");
            else if (toks[1] == "open_ms") cfg.anim.open_ms = std::stoi(toks[2]);
            else if (toks[1] == "close_ms") cfg.anim.close_ms = std::stoi(toks[2]);
            else if (toks[1] == "move_ms") cfg.anim.move_ms = std::stoi(toks[2]);
            else if (toks[1] == "easing") cfg.anim.easing = toks[2];
        } else if (cmd == "panel" && toks.size() >= 3) {
            if (toks[1] == "enabled") cfg.panel.enabled = (toks[2] == "true");
            else if (toks[1] == "position") cfg.panel.position = toks[2];
            else if (toks[1] == "height") cfg.panel.height = std::stoi(toks[2]);
            else if (toks[1] == "bg") cfg.panel.bg = toks[2];
            else if (toks[1] == "fg") cfg.panel.fg = toks[2];
        } else if (cmd == "startup" && toks.size() >= 2) {
            StartupApp a;
            size_t i = 1;
            a.cmd = take_quoted(toks, i);
            cfg.startup.push_back(a);
        } else if (cmd == "key" && toks.size() >= 3) {
            KeyBinding k;
            size_t i = 1;
            k.combo = take_quoted(toks, i);
            if (i + 1 < toks.size()) k.action = toks[++i];
            for (size_t j = i + 1; j < toks.size(); ++j) {
                std::string a = toks[j];
                if (a.front() == '"') a = take_quoted(toks, j);
                k.args.push_back(a);
            }
            cfg.keys.push_back(k);
        } else if (cmd == "workspace" && toks.size() >= 3) {
            Workspace w;
            w.id = toks[1];
            size_t i = 2;
            w.name = take_quoted(toks, i);
            cfg.workspaces.push_back(w);
        } else if (cmd == "rule" && toks.size() >= 3) {
            Rule r;
            r.app_id = toks[1];
            for (size_t i = 2; i < toks.size(); ++i) {
                if (toks[i] == "workspace" && i + 1 < toks.size())
                    r.workspace = toks[++i];
                else if (toks[i] == "float" && i + 1 < toks.size())
                    r.floating = (toks[++i] == "true");
            }
            cfg.rules.push_back(r);
        }
    }
    return cfg;
}

// Serializes the config back to the le format. Used by the settings panel
// when the user changes something at runtime.
bool save_config(const Config& cfg, const std::string& path) {
    std::ofstream f(path);
    if (!f) return false;

    f << "# LeWM config generated by the settings panel\n";
    f << "modkey \"" << cfg.modkey << "\"\n";
    f << "border_width " << cfg.border_width << "\n";
    f << "border_color_active " << cfg.border_active << "\n";
    f << "border_color_normal " << cfg.border_normal << "\n";
    f << "border_color_urgent " << cfg.border_urgent << "\n";
    f << "gap " << cfg.gap << "\n";
    f << "xwayland " << (cfg.xwayland ? "true" : "false") << "\n";
    f << "layout default \"" << cfg.default_layout << "\"\n";
    f << "animation enabled " << (cfg.anim.enabled ? "true" : "false") << "\n";
    f << "animation open_ms " << cfg.anim.open_ms << "\n";
    f << "animation close_ms " << cfg.anim.close_ms << "\n";
    f << "animation move_ms " << cfg.anim.move_ms << "\n";
    f << "animation easing \"" << cfg.anim.easing << "\"\n";
    f << "panel enabled " << (cfg.panel.enabled ? "true" : "false") << "\n";
    f << "panel position \"" << cfg.panel.position << "\"\n";
    f << "panel height " << cfg.panel.height << "\n";
    f << "panel bg " << cfg.panel.bg << "\n";
    f << "panel fg " << cfg.panel.fg << "\n";
    for (const auto& s : cfg.startup) f << "startup \"" << s.cmd << "\"\n";
    for (const auto& k : cfg.keys)
        f << "key \"" << k.combo << "\" " << k.action << "\n";
    for (const auto& w : cfg.workspaces)
        f << "workspace \"" << w.id << "\" \"" << w.name << "\"\n";
    for (const auto& r : cfg.rules)
        f << "rule \"" << r.app_id << "\" workspace \"" << r.workspace
          << "\" float " << (r.floating ? "true" : "false") << "\n";
    return true;
}

Config load_config(const std::string& path) {
    return Parser(path).parse();
}

} // namespace lewm
