#pragma once

#include <unordered_map>

#include "async.h"
#include "bulk_impl.hpp"

namespace async {

// ------------------------------------------------------------------
class asyncer {
using bulks = std::unordered_map<void*, bulk_ptr>;
bulks bulks_;

public: 
handle_t connect(std::size_t size) {
    auto bulk = std::make_unique<bulk_commands>(size);

// TODO add printers to bulk
    handle_t result = bulk.get();
    bulks_.insert({result, std::move(bulk)});
    return result;
}

void receive(handle_t handle, const char */*data*/, std::size_t /*size*/) {
    auto bulk = bulks_.find(handle);

    if (bulk == bulks_.end()) {
        throw std::runtime_error("attempt to receive data in unexistent handle");
    }

// TODO pass data to bulk
//  bulk->second->notify();
}

void disconnect(handle_t handle) {
    const auto removed = bulks_.erase(handle);

    if (removed == 0) {
        throw std::runtime_error("attempt to disconnect nonexistent handle");
    }
}

};
    
} // namespace async
