#pragma once

#include <unordered_map>
#include <sstream>
#include <atomic>
#include <thread>
#include <mutex>

#include "async.h"
#include "bulk_impl.hpp"

namespace async {

// ------------------------------------------------------------------
class asyncer {
using bulks = std::unordered_map<void*, bulk_ptr>;
bulks bulks_;
std::atomic_bool stop_{false};
std::thread bulk_handling_thread_;
std::mutex mut_;

public:

// ------------------------------------------------------------------
asyncer() {
   bulk_handling_thread_ = std::thread(&asyncer::process_loop, this);
}

// ------------------------------------------------------------------
~asyncer() {
   stop_.store(true);

   if (bulk_handling_thread_.joinable()) {
      bulk_handling_thread_.join();
   }
}

// ------------------------------------------------------------------
handle_t connect(std::size_t size) {
    std::lock_guard<std::mutex> lock(mut_);
    auto bulk = std::make_unique<bulk_commands>(size);

    handle_t result = bulk.get();
    bulks_.insert({result, std::move(bulk)});
    return result;
}

// ------------------------------------------------------------------
void receive(handle_t handle, const char* data, std::size_t) {
    std::lock_guard<std::mutex> lock(mut_);
    auto bulk = bulks_.find(handle);

    if (bulk == bulks_.end()) {
        throw std::runtime_error("attempt to receive data in nonexistent handle");
    }
    bulk->second->add_raw_commands(data);
}

// ------------------------------------------------------------------
void disconnect(handle_t handle) {
    std::lock_guard<std::mutex> lock(mut_);
    const auto removed = bulks_.erase(handle);

    if (removed == 0) {
        throw std::runtime_error("attempt to disconnect nonexistent handle");
    }
}

// ------------------------------------------------------------------
void process_loop() {

   while(!stop_) {
       std::lock_guard<std::mutex> lock(mut_);
       for (auto& bulk: bulks_) {
          bulk.second->process();
       }
   }
   std::cout << "stop loop...\n";
}

}; // asyncer
    
} // namespace async
