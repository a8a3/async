#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>


// ------------------------------------------------------------------
class command {
public:

   enum class command_type {
      fixed = 0,
      free
   };

   static constexpr auto block_start = "{";
   static constexpr auto block_end   = "}";

   command(command_type type) : now_(std::chrono::system_clock::now())
                              , type_(type) {}

   virtual bool is_full() const = 0;
   virtual void add_subcommand(const std::string& subcommand) = 0;
   virtual ~command() = default;

   using sub_commands = std::vector<std::string>;
   const sub_commands& get_sub_commands() const {
      return sub_commands_;
   }
   using creation_time_point = std::chrono::time_point<std::chrono::system_clock>;

   const creation_time_point& get_creation_time_point() const {
      return now_;
   }

   command_type get_type() const {return type_;}

protected:
   sub_commands sub_commands_;

private:
   const creation_time_point now_;
   const command_type        type_;
};
using command_ptr = std::unique_ptr<command>;

// ------------------------------------------------------------------
class fixed_size_command  : public command {
   const size_t command_size_;

public:
   fixed_size_command(size_t command_size)
   : command(command_type::fixed)
   , command_size_(command_size) {
      sub_commands_.reserve(command_size_);
   }

   bool is_full() const override {
      return sub_commands_.size() == command_size_;
   };

   void add_subcommand(const std::string& subcommand) override {
      if (is_full() || subcommand == block_start) {
         return;
      }
      sub_commands_.push_back(subcommand);
   }
};

// ------------------------------------------------------------------
class free_size_command : public command {
   size_t open_brackets_count_{1};

public:   
   free_size_command() : command(command_type::free) {}

   bool is_full() const override {
      return open_brackets_count_ == 0;
   };

   void add_subcommand(const std::string& subcommand) override {
      if (is_full()) {
         return;
      }

      if (subcommand == block_start) {
         ++open_brackets_count_;
      } else if (subcommand == block_end) {
         --open_brackets_count_;
      } else {
         sub_commands_.push_back(subcommand);
      }
   }
};
