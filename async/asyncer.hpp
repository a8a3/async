#pragma once

#include <unordered_map>
#include <sstream>

#include "async.h"
#include "bulk_impl.hpp"

namespace async {

// ------------------------------------------------------------------
class asyncer {
using bulks = std::unordered_map<void*, bulk_ptr>;
bulks bulks_;

// TODO synchronize commands to get ability to call it 
//      from different threads...

public: 

// ------------------------------------------------------------------
handle_t connect(std::size_t size) {
    auto bulk = std::make_unique<bulk_commands>(size);

    handle_t result = bulk.get();
    bulks_.insert({result, std::move(bulk)});
    return result;
}

// ------------------------------------------------------------------
void receive(handle_t handle, const char* data, std::size_t) {
    auto bulk = bulks_.find(handle);

    if (bulk == bulks_.end()) {
        throw std::runtime_error("attempt to receive data in unexistent handle");
    }

    std::stringstream ss(data);
    std::string item;
    constexpr auto sep{'\n'};
    while (std::getline(ss, item, sep)) {
        bulk->second->process(item);
    }
}

// ------------------------------------------------------------------
void disconnect(handle_t handle) {
    const auto removed = bulks_.erase(handle);

    if (removed == 0) {
        throw std::runtime_error("attempt to disconnect nonexistent handle");
    }
}
}; // asyncer
    
} // namespace async
