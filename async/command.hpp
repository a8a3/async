#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>


// ------------------------------------------------------------------
class command {
public:
   static constexpr auto block_start = "{";
   static constexpr auto block_end   = "}";

   command() : now_(std::chrono::system_clock::now()) {}

   virtual bool add_subcommand(const std::string& subcommand) = 0;
   virtual ~command() = default;

   using sub_commands = std::vector<std::string>;
   const sub_commands& get_sub_commands() const {
      return sub_commands_;
   }
   using creation_time_point = std::chrono::time_point<std::chrono::system_clock>;

   const creation_time_point& get_creation_time_point() const {
      return now_;
   }

protected:
   sub_commands sub_commands_;

private:
   const creation_time_point now_;
};
using command_ptr = std::unique_ptr<command>;

// ------------------------------------------------------------------
class fixed_size_command  : public command {
   const size_t command_size_;

public:
   fixed_size_command(size_t command_size)
   : command_size_(command_size) {
     sub_commands_.reserve(command_size_);
   }

   bool add_subcommand(const std::string& subcommand) override {
      if(subcommand == block_start) {
         return false;
      }

      sub_commands_.push_back(subcommand);
      return sub_commands_.size() != command_size_;
   }
};

// ------------------------------------------------------------------
class free_size_command : public command {
   size_t open_brackets_count_{1};

public:   
   free_size_command() = default;

   bool add_subcommand(const std::string& subcommand) override {
      if (subcommand == block_start) {
         ++open_brackets_count_;
      } else if (subcommand == block_end) {
         --open_brackets_count_;
      } else {
         sub_commands_.push_back(subcommand);
      }
      return open_brackets_count_ != 0;
   }
};
