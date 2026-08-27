#ifndef LEWM_LE_PARSER_HPP
#define LEWM_LE_PARSER_HPP

#include "le_ast.hpp"
#include <string>
#include <optional>

namespace le {

// Parses a config.le file into a Config struct.
// Throws std::runtime_error on syntax errors with a line reference.
class Parser {
public:
    explicit Parser(const std::string& path);
    Config parse();

private:
    std::string source_;
    std::vector<std::string> lines_;
    std::vector<std::string> tokens_;
    std::size_t pos_ = 0;
};

// Convenience wrapper used by the compositor at startup.
Config load_config(const std::string& path);

} // namespace le

#endif
