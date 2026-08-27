#include "ipc.hpp"

#include <stdexcept>

namespace le {

Ipc::Ipc(const std::string& runtime_dir) {
    socket_path_ = runtime_dir + "/LeWM.sock";
    fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_ < 0) throw std::runtime_error("ipc socket create failed");

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path_.c_str(), sizeof(addr.sun_path) - 1);

    if (bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error("ipc bind failed: " + socket_path_);
    if (listen(fd_, 8) < 0)
        throw std::runtime_error("ipc listen failed");
}

Ipc::~Ipc() {
    if (fd_ >= 0) close(fd_);
    unlink(socket_path_.c_str());
}

void Ipc::serve() {
    // Accept loop dispatches commands to the tiler. Stubbed: a real build
    // registers this fd with the wl_display event loop instead.
}

} // namespace le
