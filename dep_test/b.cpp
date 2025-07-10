#include <iostream>

// Declare the functions from liba.so
extern "C" {
    void example_function_a();
    int add_numbers(int a, int b);
}

extern "C" {
    void example_function_b() {
        std::cout << "Hello from libb.so - example_function_b()" << std::endl;
        std::cout << "Calling function from liba.so..." << std::endl;
        example_function_a();
    }
    
    int multiply_numbers(int a, int b) {
        std::cout << "libb.so: multiply_numbers(" << a << ", " << b << ")" << std::endl;
        // Use add_numbers from liba.so internally
        int result = 0;
        for (int i = 0; i < b; i++) {
            result = add_numbers(result, a);
        }
        return result;
    }
} 