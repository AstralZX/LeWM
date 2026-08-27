#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

namespace le {

// dwl-compatible IPC socket. Listens on $XDG_RUNTIME_DIR/LeWM.sock and
// accepts single-line commands (layout_next, kill_focused, ...). The wire
// format matches dwl's ipc so existing clients work unchanged.
class Ipc {
public:
    explicit Ipc(const std::string& runtime_dir);
    ~Ipc();

    Ipc(const Ipc&) = delete;
    Ipc& operator=(const Ipc&) = delete;

    void serve();

private:
    std::string socket_path_;
    int fd_ = -1;
};

} // namespace le
