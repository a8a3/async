#pragma once

#include <fstream>
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
   std::stringstream ss_;

public:
   using printer = std::function<void(const command_ptr&)>;
   
   bulk_commands(size_t bulk_size) : bulk_size_(bulk_size) {

      const auto console_printer = [](const command_ptr& cmd) {
         std::cout << *cmd << std::endl;
      };

      const auto file_printer = [](const command_ptr& cmd) {
         const auto cmd_creation_time = std::chrono::duration_cast<std::chrono::microseconds>(cmd->get_creation_time_point().time_since_epoch());
         const auto file_name = "bulk_" + std::to_string(cmd_creation_time.count()) + ".txt";
         std::ofstream file(file_name, std::ios::app);
         file << *cmd;
         file.close();
      };

      printers_.reserve(2);
      printers_.push_back(console_printer);
      printers_.push_back(file_printer);
   }
   ~bulk_commands() {
      if (current_command_) {
         out_command(current_command_);
      }
   }

   void add_raw_commands(const std::string& raw_commands) {
      if (ss_.eof()) {
         ss_.clear();

         if (raw_commands.front() == '\n') {
            ss_ << raw_commands.substr(1);
            return;
         }
      }
      ss_ << raw_commands;
   }

   command_ptr create_command(const std::string& token) {
      if (token == command::block_start) {
         return std::make_unique<free_size_command>();
      }
      return std::make_unique<fixed_size_command>(bulk_size_);
   }

   void out_command(const command_ptr& cmd) const {
      if(cmd->get_sub_commands().empty()){
         return;
      }
      
      for(const auto& printer: printers_) {
         printer(cmd);
      }
   }

   void process() {
      if (ss_.eof()) {
         return;
      }

      std::string str;
      std::getline(ss_, str);

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

   using printers = std::vector<printer>;
   printers printers_;
};

using bulk_ptr = std::unique_ptr<bulk_commands>;
