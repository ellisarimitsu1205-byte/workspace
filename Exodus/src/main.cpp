#include "snake.hpp"
#include <iostream>

int main() {
    Snake s({5, 5}, 3);
    std::cout << "snake size: " << s.size() << "\n";
}