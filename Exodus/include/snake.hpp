#pragma once
#include <deque>
#include <unordered_set>
#include <cstddef>

struct Point {
    int x = 0;
    int y = 0;
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

struct PointHash {
    std::size_t operator()(const Point& p) const noexcept {
        return static_cast<std::size_t>(p.x) * 73856093u
             ^ static_cast<std::size_t>(p.y) * 19349663u;
    }
};

class Snake {
public:
    Snake(Point start, int len);

    void step(Point dir, bool grow);
    bool occupies(Point p) const {return cells_.count(p) > 0; }
    Point head() const { return body_.front(); }
    Point tail() const { return body_.back(); }
    std::size_t size() const { return body_.size(); }

    auto begin() const { return body_.begin(); }
    auto end() const { return body_.end(); }
private:
    std::deque<Point> body_;
    std::unordered_set<Point, PointHash> cells_;
};