#ifndef LEWM_IPC_HPP
#define LEWM_IPC_HPP

#include <string>
#include <functional>

namespace lewm {

class Ipc {
public:
    using Handler = std::function<void(const std::string&)>;

    Ipc(const std::string& runtime_dir, Handler handler);
    ~Ipc();

    Ipc(const Ipc&) = delete;
    Ipc& operator=(const Ipc&) = delete;

    bool serve();

private:
    std::string socket_path_;
    int fd_ = -1;
    Handler handler_;
};

} // namespace lewm

#endif
