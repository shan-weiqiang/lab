#include <asio.hpp>
#include <asio/bind_executor.hpp>
#include <asio/detail/chrono.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <asio/strand.hpp>
#include <functional>
#include <iostream>
#include <thread>

class printer {

public:
  printer(asio::io_context &io)
      : cnt{0}, strand{asio::make_strand(io)},
        timer1{io, asio::chrono::seconds(1)},
        timer2{io, asio::chrono::seconds(1)} {
    // handlers bound to the same strand will not be executed concurrently by
    // io_context
    timer1.async_wait(
        asio::bind_executor(strand, std::bind(&printer::print1, this)));
    timer2.async_wait(
        asio::bind_executor(strand, std::bind(&printer::print2, this)));
  }

  void print1() {
    if (cnt < 1000) {
      ++cnt;
      std::cout << cnt << " ; thread id: " << std::this_thread::get_id()
                << std::endl;
      timer1.expires_at(timer1.expiry() + asio::chrono::seconds(1));
      timer1.async_wait(asio::bind_executor(
          strand, std::bind(&printer::print1, this)));
    }
  }
  void print2() {
    if (cnt < 1000) {
      ++cnt;
      std::cout << cnt << " ; thread id: " << std::this_thread::get_id()
                << std::endl;
      timer2.expires_at(timer2.expiry() + asio::chrono::seconds(1));
      timer2.async_wait(asio::bind_executor(
          strand, std::bind(&printer::print2, this)));
    }
  }

  ~printer() { std::cout << "final cnt: " << cnt << std::endl; }

private:
  int cnt;
  asio::steady_timer timer1;
  asio::steady_timer timer2;
  asio::strand<asio::io_context::executor_type> strand;
};

int main() {
  asio::io_context io;
  printer p{io};

  std::thread t{[&io]() { io.run(); }};
  io.run();
  t.join();
}