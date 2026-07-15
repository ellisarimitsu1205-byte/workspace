#include "snake.hpp"

Snake::Snake(Point start, int len) 
{
    for(int i = 0; i < len; ++i)
    {
        Point p{start.x - i, start.y};
        body_.push_back(p);
        cells_.insert(p);
    }
}

void Snake::step(Point dir, bool grow)
{
    Point next{body_.front().x + dir.x, body_.front().y + dir.y};

    if(!grow)
    {
        cells_.erase(body_.back());
        body_.pop_back();
    }
    body_.push_front(next);
    cells_.insert(next);
}