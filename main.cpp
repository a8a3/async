#include <iostream>
#include <thread>

#include "async.h"

// ------------------------------------------------------------------
int main(int, char *[]) {
    std::size_t bulk = 3;
    auto h = async::connect(bulk);
    auto h2 = async::connect(bulk);
    async::receive(h, "1", 1);
    async::receive(h2, "1\n", 2);
    async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
    async::receive(h, "b\nc\nd\n}\n89\n", 11);


    std::thread t{
        [] () {
        std::size_t bulk1 = 3;
        auto h3 = async::connect(bulk1);
        async::receive(h3, "100\n200\n300\n400\n500\n600\n700\n800\n900\n1000\n1100\n1200\n1300\n", 1);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        async::disconnect(h3);
    }};

    if (t.joinable()) {
        t.join();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    async::disconnect(h);
    async::disconnect(h2);

    return 0;
}
