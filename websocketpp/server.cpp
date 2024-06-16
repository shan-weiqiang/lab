#include "websocketpp/common/connection_hdl.hpp"
#include "websocketpp/frame.hpp"
#include <string>
#include <thread>
#include <vector>
#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

std::vector<websocketpp::connection_hdl> clients;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

// Define a callback to handle incoming messages
void on_message(server *s, websocketpp::connection_hdl hdl, message_ptr msg) {
  std::cout << "client: " << hdl.lock().get()
            << "; msg content: " << msg->get_payload()
            << "server thread id: " << std::this_thread::get_id() << std::endl;

  // check for a special command to instruct the server to stop listening so
  // it can be cleanly exited.
  if (msg->get_payload() == "stop-listening") {
    s->stop_listening();
    return;
  }
}

void on_open(websocketpp::connection_hdl hdl) {
  std::cout << "<<<<<<<" << std::endl
            << "on_open called with hdl: " << hdl.lock().get() << std::endl
            << "<<<<<<<<<<" << std::endl;
  clients.push_back(hdl);
}

int main() {
  // Create a server endpoint
  server echo_server;

  try {
    // Set logging settings
    echo_server.set_access_channels(websocketpp::log::alevel::all);
    echo_server.clear_access_channels(websocketpp::log::alevel::all);

    // Initialize Asio
    echo_server.init_asio();

    // Register our message handler
    echo_server.set_message_handler(
        bind(&on_message, &echo_server, ::_1, ::_2));
    echo_server.set_open_handler(on_open);

    // Listen on port 9002; server can only listen one one port at the same
    // time!!! but can get connected by multi clients
    echo_server.listen(9002);

    // Start the server accept loop
    echo_server.start_accept();

    // Start the ASIO io_service run loop
    std::thread t = std::thread([&echo_server]() { echo_server.run(); });
    std::thread t1 = std::thread([&echo_server]() { echo_server.run(); });

    std::string input;
    while (input != "exit") {
      std::getline(std::cin, input);
      for (websocketpp::connection_hdl &hdl : clients) {
        echo_server.send(hdl, input, websocketpp::frame::opcode::text);
      }
    }

    t.join();
    t1.join();
  } catch (websocketpp::exception const &e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "other exception" << std::endl;
  }
}
