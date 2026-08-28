#include "ipc.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
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

    unlink(socket_path_.c_str());
    if (bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error("ipc bind failed: " + socket_path_);
    if (listen(fd_, 8) < 0)
        throw std::runtime_error("ipc listen failed");

    int flags = fcntl(fd_, F_GETFL, 0);
    fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
}

Ipc::~Ipc() {
    if (fd_ >= 0) close(fd_);
    unlink(socket_path_.c_str());
}

// Poll the listening socket once. Accepts a pending client, reads one command
// line, dispatches it to the handler, then closes the client. Returns true if
// any command was handled. Meant to be called periodically from the main loop.
bool Ipc::serve() {
    if (fd_ < 0) return false;

    pollfd pfd { fd_, POLLIN, 0 };
    if (poll(&pfd, 1, 0) <= 0)
        return false;

    const int client = accept(fd_, nullptr, nullptr);
    if (client < 0)
        return false;

    std::string buf;
    char chunk[256];
    ssize_t n;
    while ((n = read(client, chunk, sizeof(chunk))) > 0)
        buf.append(chunk, static_cast<size_t>(n));

    close(client);

    std::string line = buf;
    auto nl = line.find('\n');
    if (nl != std::string::npos)
        line.resize(nl);
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
        line.pop_back();

    if (!line.empty() && handler_)
        handler_(line);

    return true;
}

} // namespace lewm
