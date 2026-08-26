#include <cstdint>
#include <cstddef>
#include <algorithm>

class PriceLadder
{
private:
    static constexpr size_t MAX_LEVELS = 1024;  // covers the price range
    static constexpr uint64_t TICK     = 1;     // 1 = prices already in ticks

    uint32_t levels_[MAX_LEVELS]{};   // quantity at each price tick
    uint64_t min_price_;              // base price — index 0
    size_t   best_bid_idx_{ 0 };      // track best bid index explicitly
    size_t   best_ask_idx_{ MAX_LEVELS - 1 }; // track best ask index

public:
    explicit PriceLadder(uint64_t min_price)
        : min_price_(min_price) {}

    // Convert price to array index
    size_t to_index(uint64_t price) const {
        return static_cast<size_t>(price - min_price_);
    }

    uint64_t to_price(size_t index) const {
        return min_price_ + index;
    }

    void add_bid(uint64_t price, uint32_t qty)
    {
        size_t idx = to_index(price);
        levels_[idx] += qty;
        best_bid_idx_ = std::max(best_bid_idx_, idx);  // O(1) update
    }

    void add_ask(uint64_t price, uint32_t qty)
    {
        size_t idx = to_index(price);
        levels_[idx] += qty;
        best_ask_idx_ = std::min(best_ask_idx_, idx);  // O(1) update
    }

    void cancel_bid(uint64_t price, uint32_t qty)
    {
        size_t idx = to_index(price);
        levels_[idx] -= qty;

        // If best level emptied, walk down to find new best
        if (levels_[best_bid_idx_] == 0)
            while (best_bid_idx_ > 0 && levels_[--best_bid_idx_] == 0);
    }

    void cancel_ask(uint64_t price, uint32_t qty)
    {
        size_t idx = to_index(price);
        levels_[idx] -= qty;

        // If best level emptied, walk up to find new best
        if (levels_[best_ask_idx_] == 0)
            while (best_ask_idx_ < MAX_LEVELS - 1 && levels_[++best_ask_idx_] == 0);
    }

    uint64_t best_bid() const { return to_price(best_bid_idx_); }
    uint64_t best_ask() const { return to_price(best_ask_idx_); }
    uint32_t bid_qty()  const { return levels_[best_bid_idx_];  }
    uint32_t ask_qty()  const { return levels_[best_ask_idx_];  }
};