#pragma once

#include <iostream>
#include <string>
#include <list>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>
#include <chrono>

#include "command.hpp"


// ------------------------------------------------------------------
class bulk_commands {
public:
   using printer = std::function<void(const command_ptr&)>;
   
   bulk_commands(size_t bulk_size) : bulk_size_(bulk_size) {
      const auto console_printer = [](const command_ptr& cmd) {
         std::cout << *cmd << std::endl;
      };

      // TODO
      const auto file_printer = [](const command_ptr&) {

      };

      add_printer(console_printer);
      add_printer(file_printer);
   }
  ~bulk_commands() {
      if (current_command_) {
         out_command(current_command_);
      }
   }

   command_ptr create_command(const std::string& token) {
      if (token == command::block_start) {
         return std::make_unique<free_size_command>();
      }
      return std::make_unique<fixed_size_command>(bulk_size_);
   }

   void add_printer(const printer& prn) {
      printers_.push_back(prn);
   }

   void out_command(const command_ptr& cmd) const {
      if(cmd->get_sub_commands().empty()){
         return;
      }
      
      for(const auto& printer: printers_) {
         printer(cmd);
      }
   }

   void process(const std::string& str) {
      if(!current_command_){
         current_command_ = create_command(str);
      }

      if (!current_command_->add_subcommand(str)) {
         out_command(current_command_);
         current_command_ = create_command(str);
      } 
   }

private:
   command_ptr current_command_;
   const size_t bulk_size_;

   using printers = std::list<printer>; 
   printers printers_;
};

using bulk_ptr = std::unique_ptr<bulk_commands>;
