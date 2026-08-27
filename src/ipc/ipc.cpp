#include "ipc.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

namespace lewm {

Ipc::Ipc(const std::string& runtime_dir, Handler handler)
    : handler_(std::move(handler)) {
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
    // A real build registers fd_ with the compositor event loop and reads
    // lines, forwarding each to handler_. Kept simple here.
}

} // namespace lewm
