#include <boost/asio.hpp>
#include <boost/asio/detail/chrono.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>
#include <chrono>
#include <iostream>
#include <thread>

void print(const boost::system::error_code) {
  std::cout << "Hello World!" << std::endl;
}

int main() {
  boost::asio::io_context io;
  //   timer start
  boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
  {
    std::this_thread::sleep_for(boost::asio::chrono::seconds(3));
    auto n = boost::asio::steady_timer::clock_type::now();
    std::cout << "timer left: "
              << std::chrono::duration_cast<boost::asio::chrono::milliseconds>(
                     t.expiry() - n)
                     .count()
              << std::endl;
  }
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