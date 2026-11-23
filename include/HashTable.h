#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>

struct HashEntry {
    std::string key;
    std::string value;
    HashEntry* next;
    HashEntry(const std::string& k, const std::string& v) : key(k), value(v), next(nullptr) {}
};

class HashTable {
private:
    static const int TABLE_SIZE = 16;
    HashEntry* buckets[TABLE_SIZE];
    int hashFunction(const std::string& key) const;

public:
    HashTable();
    ~HashTable();
    void insert(const std::string& key, const std::string& value);
    std::string search(const std::string& key) const;
    bool remove(const std::string& key);
    void display() const;
};

#endif