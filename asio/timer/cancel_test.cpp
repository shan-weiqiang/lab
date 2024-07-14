#include <asio.hpp>
#include <iostream>
#include <thread>

// demonstrate that cancle will only effect expiry waiting handlers BEFORE the call, NOT influence async_wait AFTER the call

void handler(const std::error_code& ec) {
    if (ec == asio::error::operation_aborted) {
        std::cout << "Timer operation was canceled.\n";
    } else {
        std::cout << "Timer expired.\n";
    }
}

int main() {
    asio::io_context io_context;
    asio::steady_timer timer(io_context, std::chrono::seconds(2));

    // First async_wait
    timer.async_wait([](const std::error_code& ec) {
        if (ec == asio::error::operation_aborted) {
            std::cout << "First async_wait operation was canceled.\n";
        } else {
            std::cout << "First async_wait timer expired.\n";
        }
    });

    // Cancel the timer after 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    timer.cancel();

    // Second async_wait
    timer.expires_after(std::chrono::seconds(2));
    timer.async_wait([](const std::error_code& ec) {
        if (ec == asio::error::operation_aborted) {
            std::cout << "Second async_wait operation was canceled.\n";
        } else {
            std::cout << "Second async_wait timer expired.\n";
        }
    });

    io_context.run();

    return 0;
}