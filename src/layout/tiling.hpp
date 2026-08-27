#ifndef LEWM_TILING_HPP
#define LEWM_TILING_HPP

#include <string>
#include <vector>

namespace le {

// Static tiling engine. Keeps a list of managed surfaces per workspace and
// arranges them with the active layout. The engine is intentionally dumb:
// no animations, no gaps math beyond what the config asks for.
class Tiler {
public:
    Tiler();

    void add_window(const std::string& id);
    void remove_window(const std::string& id);
    void focus(const std::string& id);

    void cycle_layout();
    const std::string& current_layout() const { return layout_; }

    // Returns the window ids in their tiled order for the current layout.
    const std::vector<std::string>& order() const { return order_; }

private:
    std::vector<std::string> order_;
    std::string layout_ = "tile";
    std::vector<std::string> layouts_ = {"tile", "mono", "grid"};
};

} // namespace le

#endif
