#include <cstdint>
#include <deque>
#include <map>
#include <unordered_map>
#include <functional>
#include <stdexcept>

using Price    = int64_t;
using Quantity = int32_t;
using OrderId  = uint64_t;

static constexpr int64_t PRICE_SCALE = 10000;

enum class Side { Bid, Ask };

struct Order {
    OrderId  id;
    Price    price;
    Quantity quantity;
    Side     side;
};

struct OrderLocation {
    Price                          price;
    Side                           side;
    std::deque<Order>::iterator    pos;   
};

class OrderBook {
    std::map<Price, std::deque<Order>, std::greater<Price>> bids_;  
    std::map<Price, std::deque<Order>>                      asks_
    std::unordered_map<OrderId, OrderLocation>              order_index_;

public:
    void add_order(Order o)
    {
        auto& levels = (o.side == Side::Bid) ? bids_ : asks_;
        auto& dq     = levels[o.price];      
        dq.push_back(o);                      

        order_index_[o.id] = {
            o.price,
            o.side,
            std::prev(dq.end())              
        };
    }

    void cancel_order(OrderId id)
    {
        auto it = order_index_.find(id);
        if (it == order_index_.end()) return;  // unknown order, ignore

        auto& [price, side, pos] = it->second;
        auto& levels = (side == Side::Bid) ? bids_ : asks_;
        auto  level  = levels.find(price);

        if (level != levels.end())
        {
            level->second.erase(pos);          
            if (level->second.empty())
                levels.erase(level);          
        }

        order_index_.erase(it);
    }

    Price best_bid() const
    {
        if (bids_.empty()) return -1;
        return bids_.begin()->first;            
    }

    Price best_ask() const
    {
        if (asks_.empty()) return -1;
        return asks_.begin()->first;            
    }

    Quantity bid_quantity(Price p) const
    {
        auto it = bids_.find(p);
        if (it == bids_.end()) return 0;
        Quantity total = 0;
        for (const auto& o : it->second) total += o.quantity;
        return total;
    }

    Quantity ask_quantity(Price p) const
    {
        auto it = asks_.find(p);
        if (it == asks_.end()) return 0;
        Quantity total = 0;
        for (const auto& o : it->second) total += o.quantity;
        return total;
    }

    void match_orders()
    {
    while (!bids_.empty() && !asks_.empty())
    {
        Price bid_price = bids_.begin()->first;
        Price ask_price = asks_.begin()->first;

        if (bid_price < ask_price) break;  
        
        auto& bid_dq = bids_.begin()->second;
        auto& ask_dq = asks_.begin()->second;

        Order& bid = bid_dq.front(); 
        Order& ask = ask_dq.front(); 

        Quantity fill = std::min(bid.quantity, ask.quantity);

        bid.quantity -= fill;
        ask.quantity -= fill;

        // fully filled orders leave the book
        if (bid.quantity == 0) {
            order_index_.erase(bid.id);
            bid_dq.pop_front();
        }
        if (ask.quantity == 0) {
            order_index_.erase(ask.id);
            ask_dq.pop_front();
        }

        // clean up empty price levels
        if (bid_dq.empty()) bids_.erase(bids_.begin());
        if (ask_dq.empty()) asks_.erase(asks_.begin());
    }
    }
};