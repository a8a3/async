#include "async.h"
#include "asyncer.hpp"

namespace async {

namespace {
asyncer g_asyncer;
}

handle_t connect(std::size_t bulk) {
    return g_asyncer.connect(bulk);
}

void receive(handle_t handle, const char* data, std::size_t size) {
    return g_asyncer.receive(handle, data, size);
}

void disconnect(handle_t handle) {
    g_asyncer.disconnect(handle);
}

} // namespace async
