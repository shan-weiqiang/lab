#include <iostream>

// Declare the functions from libb.so
extern "C" {
    void example_function_b();
    int multiply_numbers(int a, int b);
}

int main() {
    std::cout << "=== Testing Library Dependencies ===" << std::endl;
    
    std::cout << "\n1. Calling example_function_b() from libb.so:" << std::endl;
    example_function_b();
    
    std::cout << "\n2. Calling multiply_numbers(5, 3) from libb.so:" << std::endl;
    int result = multiply_numbers(5, 3);
    std::cout << "Result: " << result << std::endl;
    
    std::cout << "\n=== Test completed ===" << std::endl;
    return 0;
} 