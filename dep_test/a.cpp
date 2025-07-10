#include <iostream>

extern "C" {
    void example_function_a() {
        std::cout << "Hello from liba.so - example_function_a()" << std::endl;
    }
    
    int add_numbers(int a, int b) {
        return a + b;
    }
} 