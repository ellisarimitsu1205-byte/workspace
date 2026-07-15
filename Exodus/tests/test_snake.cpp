#include "snake.hpp"
#include <cassert>
#include <iostream>

int main() {
    Snake s({5, 5}, 3);          
    assert(s.size() == 3);
    assert(s.occupies({3, 5}));
    assert(!s.occupies({6, 5}));

    s.step({1, 0}, false);         
    assert(s.head() == (Point{6, 5}));
    assert(s.size() == 3);
    assert(!s.occupies({3, 5}));   

    s.step({1, 0}, true);          
    assert(s.size() == 4);
    assert(s.occupies({4, 5}));    

    // the tail-follow case
    Snake t({5, 5}, 3);
    Point vacating = t.tail();
    t.step({0, 1}, false);
    assert(!t.occupies(vacating));

    std::cout << "so far so good ig";
}