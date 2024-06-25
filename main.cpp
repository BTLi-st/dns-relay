#include <thread>
#include <chrono>
#include "log.h"

int main()
{
    Log log(FilePath().add_dir("log").add_file("server.log"), DEBUG);
    log.info("Hello, World!");
    std::thread t1([&log] {
        log.debug("Debug from thread1.");
        log.info("Message from thread1.");
        log.warning("Warning from thread1.");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        log.error("Error from thread1.");
        log.fatal("Fatal from thread1.");
    });
    std::thread t2([&log] {
        log.debug("Debug from thread2.");
        log.info("Message from thread2.");
        log.warning("Warning from thread2.");
        log.error("Error from thread2.");
        log.fatal("Fatal from thread2.");
    });
    t1.join();
    t2.join();
    return 0;
}