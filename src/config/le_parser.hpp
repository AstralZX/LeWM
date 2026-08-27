#ifndef LEWM_LE_PARSER_HPP
#define LEWM_LE_PARSER_HPP

#include "le_ast.hpp"
#include <string>

namespace lewm {

class Parser {
public:
    explicit Parser(const std::string& path) : path_(path) {}
    Config parse();

private:
    std::string path_;
};

Config load_config(const std::string& path);
bool save_config(const Config& cfg, const std::string& path);

} // namespace lewm

#endif
