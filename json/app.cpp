#include  "json.hpp"
#include <fstream>
#include <iostream>

int main(){
    std::fstream f("file.json");
    nlohmann::json data = nlohmann::json::parse(f);
    std::cout << data << std::endl;
}