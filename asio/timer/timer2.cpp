#include <asio.hpp>
#include <asio/detail/chrono.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <chrono>
#include <iostream>
#include <thread>

void print(const std::error_code) {
  std::cout << "print thread id: " << std::this_thread::get_id() << std::endl;
}

int main() {
  std::cout << "main thread id: " << std::this_thread::get_id() << std::endl;
  asio::io_context io;
  //   timer start
  asio::steady_timer t(io, asio::chrono::seconds(1));
  {
    std::this_thread::sleep_for(asio::chrono::seconds(3));
    auto n = asio::steady_timer::clock_type::now();
    std::cout << "timer left: "
              << std::chrono::duration_cast<asio::chrono::milliseconds>(
                     t.expiry() - n)
                     .count()
              << std::endl;
  }
  // handler will always be executed in io.run() thread, even if when async_wait is
  // called and the timer is alread expired; this is achieved by using the timer
  // as fd and use epoll to mointor the epoll:
  // https://man7.org/linux/man-pages/man2/timerfd_create.2.html
  t.async_wait(print);

  auto now = std::chrono::system_clock::now();
  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::thread async_t = std::thread([&io, &now]() {
    io.run();
    auto time_intval = std::chrono::system_clock::now() - now;
    std::cout
        << "time passed in seconds: "
        << std::chrono::duration_cast<std::chrono::seconds>(time_intval).count()
        << std::endl;
  });
  async_t.join();
  return 0;
}
