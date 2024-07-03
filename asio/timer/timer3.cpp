#include <asio.hpp>
#include <asio/detail/chrono.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <functional>
#include <iostream>

void print(const std::error_code ec, asio::steady_timer *t,
           int *cnt) {
  if (*cnt < 5) {
    std::cout << *cnt << std::endl;
    ++(*cnt);
    t->expires_at(t->expiry() + asio::chrono::seconds(1));
    t->async_wait(std::bind(print, std::placeholders::_1, t, cnt));
  }
}

int main() {
  asio::io_context io;
  int cnt = 0;
  asio::steady_timer t(io, asio::chrono::seconds(1));
  t.async_wait(std::bind(print, std::placeholders::_1, &t, &cnt));
  io.run();
}