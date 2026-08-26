#include <unordered_map>
#include <list>
#include <optional>
#include <cstddef>

template<typename Key, typename Value>
class LRUCache
{
private:
    using ListIt = typename std::list<Key>::iterator;
    using Cache = std::unordered_map<Key, std::pair<Value, ListIt>>;

    Cache cache_;
    std::list<Key> lru_;
    std::size_t capacity_;

    void touch(typename Cache::iterator it)
    {
        lru_.splice(lru_.begin(), lru_, it->second.second);
        it->second.second = lru_.begin();
    }
public:
    LRUCache(std::size_t capacity) 
        : capacity_(capacity)
    { }

    std::optional<Value> get(const Key& key)
    {
        auto it = cache_.find(key);
        if(it == cache_.end()) return std::nullopt;
        touch(it);
        return it->second.first;
    }

    void put(const Key& key, const Value& value)
    {
        auto it = cache_.find(key);
        if(it != cache_.end())
        {
            touch(it);
            it->second.first = value;
            return;
        }
        if(cache_.size() >= capacity_)
        {
            cache_.erase(lru_.back());
            lru_.pop_back();
        }
        lru_.push_front(key);
        cache_.insert({key, {value, lru_.begin()}});
    }

    bool contains(const Key& key)
    {
        return cache_.contains(key);
    }

    bool erase(const Key& key)
    {
        auto it = cache_.find(key);
        if(it == cache_.end()) return false;
        lru_.erase(it->second.second);
        cache_.erase(it);
        return true;
    }
};