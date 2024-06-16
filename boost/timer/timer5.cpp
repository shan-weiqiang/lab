#include <boost/asio.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/detail/chrono.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <iostream>
#include <thread>

class printer {

public:
  printer(boost::asio::io_context &io)
      : cnt{0}, strand{boost::asio::make_strand(io)},
        timer1{io, boost::asio::chrono::seconds(1)},
        timer2{io, boost::asio::chrono::seconds(1)} {
    // handlers bound to the same strand will not be executed concurrently by
    // io_context
    timer1.async_wait(
        boost::asio::bind_executor(strand, std::bind(&printer::print1, this)));
    timer2.async_wait(
        boost::asio::bind_executor(strand, std::bind(&printer::print2, this)));
  }

  void print1() {
    if (cnt < 1000) {
      ++cnt;
      std::cout << cnt << " ; thread id: " << std::this_thread::get_id()
                << std::endl;
      timer1.expires_at(timer1.expiry() + boost::asio::chrono::seconds(1));
      timer1.async_wait(boost::asio::bind_executor(
          strand, std::bind(&printer::print1, this)));
    }
  }
  void print2() {
    if (cnt < 1000) {
      ++cnt;
      std::cout << cnt << " ; thread id: " << std::this_thread::get_id()
                << std::endl;
      timer2.expires_at(timer2.expiry() + boost::asio::chrono::seconds(1));
      timer2.async_wait(boost::asio::bind_executor(
          strand, std::bind(&printer::print2, this)));
    }
  }

  ~printer() { std::cout << "final cnt: " << cnt << std::endl; }

private:
  int cnt;
  boost::asio::steady_timer timer1;
  boost::asio::steady_timer timer2;
  boost::asio::strand<boost::asio::io_context::executor_type> strand;
};

int main() {
  boost::asio::io_context io;
  printer p{io};

  std::thread t{[&io]() { io.run(); }};
  io.run();
  t.join();
}