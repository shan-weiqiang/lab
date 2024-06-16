#include "websocketpp/common/connection_hdl.hpp"
#include "websocketpp/common/functional.hpp"
#include "websocketpp/common/system_error.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/error.hpp"
#include "websocketpp/logger/levels.hpp"
#include "websocketpp/roles/client_endpoint.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

std::mutex mtx;
std::condition_variable cv;
std::atomic_bool connected{false};

typedef client::message_ptr message_ptr;
void on_message(client *c, websocketpp::connection_hdl hdl, message_ptr msg) {
  std::cout << "server: " << msg->get_payload()
            << "; client thread id: " << std::this_thread::get_id()
            << std::endl;
}

int main(int argc, char *argv[]) {

  client c;
  std::string uri = "ws://localhost:9002";

  if (argc == 2) {
    uri = argv[1];
  }
  try {

    c.set_access_channels(websocketpp::log::alevel::all);
    c.clear_access_channels(websocketpp::log::alevel::all);
    c.init_asio();
    c.set_message_handler(bind(&on_message, &c, _1, _2));
    c.set_open_handler([](websocketpp::connection_hdl hdl) {
      // now main thread can send messages
      std::lock_guard<std::mutex> lck{mtx};
      connected.store(true);
      cv.notify_one();
    });
    websocketpp::lib::error_code ec;
    client::connection_ptr con = c.get_connection(uri, ec);
    // client can connect to multi servers and the same time!!!
    // client::connection_ptr con1 = c.get_connection("ws://localhost:9002",
    // ec);

    if (ec) {
      std::cout << "could not create connection because: " << ec.message()
                << std::endl;
      return 0;
    }
    c.connect(con);
    // c.connect(con1);

    std::thread t([&c]() { c.run(); });
    std::thread t1([&c]() { c.run(); });

    {
      std::unique_lock<std::mutex> lck{mtx};
      cv.wait(lck, [] { return connected.load(); });
    }
    std::string input;
    while (input != "exit") {
      std::getline(std::cin, input);
      std::cout << '\r';
      std::cout << "client: " << input << std::endl;
      fflush(stdout);
      c.send(con->get_handle(), input, websocketpp::frame::opcode::text, ec);
      if (ec) {
        std::cout << "could not send message because: " << ec.message()
                  << std::endl;
        return 0;
      }
    }
    c.stop();

    t.join();
    t1.join();

  } catch (websocketpp::exception const &e) {
    std::cout << e.what() << std::endl;
  }
}