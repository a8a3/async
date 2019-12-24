
#include <iostream>
#include <sstream>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>

// ------------------------------------------------------------------
class asyncer {
   std::stringstream ss_;
   std::atomic_bool stop_{false};
   std::thread main_loop_;
   std::mutex mut_;
public:
   asyncer() {
      main_loop_ = std::thread(&asyncer::commands_handling_loop, this);
   }

   ~asyncer() {
      stop_.store(true);

      if (main_loop_.joinable()){
         main_loop_.join();
      }
   }

   void add_commands(const std::string& cmds) {
      std::lock_guard<std::mutex> lock(mut_);

      if (ss_.eof()) {
         ss_.clear();

         if (cmds.front() == '\n') {
            ss_ << cmds.substr(1);
            return;
         }
      }

      ss_ << cmds;
   }

   void commands_handling_loop() {
      std::string item;

      while(!stop_) {
         std::unique_lock<std::mutex> lock(mut_);
         if (!ss_.eof()) {
            std::getline(ss_, item);
            std::cout << item << std::endl;
         } else {
            std::cout << "no data" << std::endl;
         }
         lock.unlock();
         std::this_thread::sleep_for(std::chrono::milliseconds (500));
      }
   }
};

// ------------------------------------------------------------------
int main() {

   asyncer a;
   a.add_commands("0\n1\n2\n3\n");
   a.add_commands("4\n5\n6\n");
   a.add_commands("7\n8\n9\n");
   a.add_commands("10\n11");
   a.add_commands("\n12\n13\n14\n15\n");
   std::this_thread::sleep_for(std::chrono::seconds(10));

   return 0;
}