#include <asio.hpp>
#include <asio/detail/chrono.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <iostream>

class printer {
public:
  printer(asio::io_context &io, int cnt, int interval, const std::string &name_)
      : cnt{cnt}, interval{interval},
        t{asio::steady_timer(io, asio::chrono::seconds(interval))}, name{
                                                                        name_} {
    t.async_wait([this](std::error_code ec) { print(ec); });
  }

  void print(std::error_code) {
    if (cnt < 5) {
      std::cout << cnt << std::endl;
      ++cnt;
      std::cout << "timer triggered, from timer: " << name << std::endl;
      //   reset timer start timepoint
      t.expires_at(t.expiry() + asio::chrono::seconds(interval));
      t.async_wait([this](std::error_code ec) { print(ec); });
    }
  }

  ~printer() { std::cout << "final cnt: " << name << ":" << cnt << std::endl; }

private:
  int cnt;
  int interval;
  asio::steady_timer t;
  std::string name;
};

int main() {
  asio::io_context io;
  printer p{io, 0, 2, "before run"};
  std::thread th = std::thread([&io]() { io.run(); });
  printer p1{io, 0, 1, "after run"};
  th.join();
}