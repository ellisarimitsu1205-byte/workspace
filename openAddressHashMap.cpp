
template <typename Key, typename Value>
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
    int capacity;
    int size;

    int hash(const Key& key) const noexcept
    {
        return std::hash<Key>{}(key) % capacity;
    }
public:
    OpenAddressingHashMap(int cap = 16)
        : capacity(cap)
        , size(0)
     {
        table.resize(capacity);
    }

    void insert(const Key& key, const Value& value) {
        // TODO: Implement
        //The table must automatically resize (double capacity) when the load factor exceeds 0.5.
        if(size >= capacity / 2)
        {
            resize();
        }

        int idx = hash(key);
        //we match the hash 
        while(table[idx].isOccupied && !table[idx].isDeleted && table[idx].key != key)
        {
            //we be probin n shii
            idx = (idx + 1) % capacity;
        }

        if(!table[idx].isOccupied || table[idx].isDeleted)
        {
            table[idx].key = key;
            table[idx].value = value;

            table[idx].isOccupied = true;
            table[idx].isDeleted = false;
            ++size;
        }
        //is occupied and not delete dwe jusut change the vbalue
        else
        {
            table[idx].value = value;
        }
    }

    bool erase(const Key& key) {
        // TODO: Implement
        int idx = hash(key);
        while(table[idx].isOccupied)
        {
            if(!table[idx].isDeleted && table[idx].key == key)
            {
                table[idx].isDeleted = true;
                --size;
                return true;
            }
            idx = (idx + 1) % capacity;
        }
        return false;
    }

    Value& get(const Key& key) {
        // TODO: Implement
        int idx  = hash(key);
        while(table[idx].isOccupied)
        {
            if(!table[idx].isDeleted && table[idx].key == key)
            {
                return table[idx].value;
            }
            else
            {
                idx = (idx + 1) % capacity;
            }
        }
        throw std::out_of_range("Value does not exist");
    }

    bool contains(const Key& key) const {
        // TODO: Implement
        int idx = hash(key);
        while(table[idx].isOccupied)
        {
            if(!table[idx].isDeleted && table[idx].key == key)
            {
                return true;
            }
            else 
            {
                idx = (idx + 1) % capacity;
            }
        }
        return false;
    }

    // Change auto to whatever size type you're using (e.g. std::size_t, unsigned, etc.)
    auto get_size() const {
        // TODO: Implement
        return size;
    }

    auto get_capacity() const {
        // TODO: Implement
        return capacity;
    }
private:
    void resize()
    {
        int idx;
        int newCapacity = capacity * 2;
        std::vector<Entry> newTable(newCapacity);
        for(const auto& entry : table)
        {
            if(entry.isOccupied && !entry.isDeleted)
            {
                idx = std::hash<Key>{}(entry.key) % newCapacity;
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
