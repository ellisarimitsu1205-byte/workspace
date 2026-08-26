#include <iostream>
#include <list>
#include <cstring>




//Hashtable to implement 905, Jimmy

class HashTable {
    private:
        static const int hashGroups = 10;
        std::list<std::pair<int, std::string>> table[hashGroups];

    public:
        bool isEmpty() const;
        int hashFunction(int key);
        void insertItem(int key, std::string value);
        void removeItem(int key);
        std::string searchTable(int key);
        void printTable();
};


bool HashTable::isEmpty() const
{
    int sum{};
    for(int i{}; i < hashGroups; i++)
    {
        sum += table[i].size();
    }

    if(!sum){
        return true;
    }
    return false;
}


int HashTable::hashFunction(int key)
{
    return key % hashGroups; //905 this function will spit out 5
}

void HashTable::insertItem(int key, std::string value)
{
    int hashValue = hashFunction(key);
    auto& cell = table[hashValue];
    auto bItr = begin(cell);
    bool keyExists = false;
    for (; bItr !=end(cell); bItr++)
    {
        if(bItr->first == key)
        {
            keyExists = true;
            bItr->second = value;
            std::cout << "[WARNING] Key exists Value replaced" << std::endl;
            break;
        }
    }

    if(!keyExists) {
        cell.emplace_back(key, value);
    }
    return; 
}

void HashTable::removeItem(int key)
{
    int hashValue = hashFunction(key);
    auto& cell = table[hashValue];
    auto bItr = begin(cell);
    bool keyExists = false;
    for (; bItr !=end(cell); bItr++)
    {
        if(bItr->first == key)
        {
            keyExists = true;
            bItr = cell.erase(bItr);
            std::cout << "[INFO] Item Removed" << std::endl;
            break;
        }
    }
    if(!keyExists)
    {
        std::cout << "[Warning]  Key not found" << std::endl;
    }

    return;
}

void HashTable::printTable()
{
    for(int i{}; i < hashGroups; i++)
    {
        if (table[i].size() ==  0) continue;

        auto bItr = table[i].begin();
        for(; bItr != table[i].end(); bItr++)
        {
            std::cout << "[INFO] Key: " << bItr->first << "Value: " << bItr->second << std::endl;
        }
    }
}

int main()
{
    HashTable HT;

    if(HT.isEmpty())
    {
        std::cout << "Correct, good job" << std::endl;
    } else {
        std::cout << "Kill yourself" << std::endl;
    }

    HT.insertItem(901, "Jim");
    HT.insertItem(912, "Bob");
    HT.insertItem(943, "Ellis");
    HT.insertItem(254, "Stupid");
    HT.insertItem(235, "Lol");
    HT.insertItem(456, "Jb");
    HT.insertItem(677, "Jfm");
    HT.insertItem(808, "Jerm");
    HT.insertItem(919, "Jrf");
    HT.insertItem(674, "Jdfdf");
    HT.insertItem(342, "Jdfdm");
    HT.insertItem(431, "Jdfdf");
    HT.insertItem(155, "Jfdf");


    HT.printTable();

    HT.removeItem(901);
    HT.removeItem(456);

    if(HT.isEmpty())
    {
        std::cout << "KYS" << std::endl;
    } else {
        std::cout << "Nice" << std::endl;
    }
    return 0;
}