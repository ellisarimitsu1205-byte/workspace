#pragma once
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename Key, typename Value, typename Hash = std::hash<Key>>
class OpenAddressingHashMap {

private:
    struct Entry
    {
        Key key;
        Value value;
        bool isOccupied{ false };
        bool isDeleted{ false };
    };

    std::vector<Entry> table;
    std::size_t capacity;
    std::size_t size;

    template <typename Query>
    std::size_t hash(const Query& key) const
    {
        return Hash{}(key) % capacity;
    }
public:
    explicit OpenAddressingHashMap(std::size_t cap = 16)
        : capacity(cap)
        , size(0)
     {
        if(capacity == 0)
        {
            throw std::invalid_argument("Capacity must be greater than zero");
        }
        table.resize(capacity);
    }

    void insert(const Key& key, const Value& value) {
        std::size_t index = hash(key);
        std::size_t firstDeleted = capacity;

        for(std::size_t probes = 0; probes < capacity; ++probes)
        {
            Entry& entry = table[index];
            if(!entry.isOccupied)
            {
                break;
            }
            if(entry.isDeleted)
            {
                if(firstDeleted == capacity)
                {
                    firstDeleted = index;
                }
            }
            else if(entry.key == key)
            {
                entry.value = value;
                return;
            }

            index = (index + 1) % capacity;
        }

        const std::size_t newSize = size + 1;
        if(newSize > capacity - newSize)
        {
            resize();
            insert(key, value);
            return;
        }

        if(firstDeleted != capacity)
        {
            index = firstDeleted;
        }

        Entry& entry = table[index];
        entry.key = key;
        entry.value = value;
        entry.isOccupied = true;
        entry.isDeleted = false;
        ++size;
    }

    bool erase(const Key& key) {
        std::size_t index = hash(key);
        for(std::size_t probes = 0; probes < capacity; ++probes)
        {
            Entry& entry = table[index];
            if(!entry.isOccupied)
            {
                return false;
            }
            if(!entry.isDeleted && entry.key == key)
            {
                entry.isDeleted = true;
                --size;
                return true;
            }
            index = (index + 1) % capacity;
        }
        return false;
    }

    template <typename Query>
    Value* find(const Query& key)
    {
        std::size_t index = hash(key);
        for(std::size_t probes = 0; probes < capacity; ++probes)
        {
            Entry& entry = table[index];
            if(!entry.isOccupied)
            {
                return nullptr;
            }
            if(!entry.isDeleted && entry.key == key)
            {
                return &entry.value;
            }
            index = (index + 1) % capacity;
        }
        return nullptr;
    }

    Value& get(const Key& key) {
        if(Value* value = find(key))
        {
            return *value;
        }
        throw std::out_of_range("Value does not exist");
    }

    bool contains(const Key& key) const {
        std::size_t index = hash(key);
        for(std::size_t probes = 0; probes < capacity; ++probes)
        {
            const Entry& entry = table[index];
            if(!entry.isOccupied)
            {
                return false;
            }
            if(!entry.isDeleted && entry.key == key)
            {
                return true;
            }
            index = (index + 1) % capacity;
        }
        return false;
    }

    std::size_t get_size() const {
        return size;
    }

    std::size_t get_capacity() const {
        return capacity;
    }
private:
    void resize()
    {
        if(capacity > table.max_size() / 2)
        {
            throw std::length_error("Hash map capacity cannot be increased");
        }
        const std::size_t newCapacity = capacity * 2;
        std::vector<Entry> newTable(newCapacity);
        for(const auto& entry : table)
        {
            if(entry.isOccupied && !entry.isDeleted)
            {
                std::size_t idx = Hash{}(entry.key) % newCapacity;
                while(newTable[idx].isOccupied)
                {
                    idx = (idx + 1) % newCapacity;
                }
                newTable[idx] = entry;
            }
        }
        table = std::move(newTable);
        capacity = newCapacity;
    }
};
