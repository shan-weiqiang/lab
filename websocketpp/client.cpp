#include "websocketpp/common/functional.hpp"
#include "websocketpp/common/system_error.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/error.hpp"
#include "websocketpp/logger/levels.hpp"
#include "websocketpp/roles/client_endpoint.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

typedef client::message_ptr message_ptr;
void on_message(client *c, websocketpp::connection_hdl hdl, message_ptr msg) {
  std::cout << "on_message called with hdl: " << hdl.lock().get()
            << " and message: " << msg->get_payload() << std::endl;

  websocketpp::lib::error_code ec;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
  if (ec) {
    std::cout << "Echo failed because: " << ec.message() << std::endl;
  }
}

int main(int argc, char *argv[]) {

  client c;
  std::string uri = "ws://localhost:9002";
  if (argc == 2) {
    uri = argv[1];
  }
  try {
    std::string input;
    std::cout << "please input echo text: " << std::endl;
    std::cin >> input;
    c.set_access_channels(websocketpp::log::alevel::all);
    c.clear_access_channels(websocketpp::log::alevel::frame_payload);
    c.init_asio();
    c.set_message_handler(bind(&on_message, &c, _1, _2));
    websocketpp::lib::error_code ec;
    client::connection_ptr con = c.get_connection(uri, ec);
    if (ec) {
      std::cout << "could not create connection because: " << ec.message()
                << std::endl;
      return 0;
    }
    c.connect(con);

    std::thread t([&c]() { c.run(); });
    std::thread t1([](){
        std::cout << "thread test" << std::endl;
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));

    c.send(con->get_handle(), input, websocketpp::frame::opcode::text, ec);
    if (ec) {
      std::cout << "could not send message because: " << ec.message()
                << std::endl;
      return 0;
    }
    t.join();
    t1.join();

  } catch (websocketpp::exception const &e) {
    std::cout << e.what() << std::endl;
  }
}