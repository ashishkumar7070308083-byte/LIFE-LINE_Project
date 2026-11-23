#include "HashTable.h"
#include "LinkedList.h"
#include <iostream>

using namespace std;

HashTable::HashTable() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        buckets[i] = nullptr;
    }
}

HashTable::~HashTable() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        HashEntry* curr = buckets[i];
        while (curr != nullptr) {
            HashEntry* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
        buckets[i] = nullptr;
    }
}

int HashTable::hashFunction(const string& key) const {
    unsigned long hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }
    return static_cast<int>(hash % TABLE_SIZE);
}

void HashTable::insert(const string& key, const string& value) {
    int idx = hashFunction(key);
    HashEntry* curr = buckets[idx];
    while (curr != nullptr) {
        if (curr->key == key) {
            curr->value = value;
            return;
        }
        curr = curr->next;
    }
    HashEntry* newEntry = new HashEntry(key, value);
    newEntry->next = buckets[idx];
    buckets[idx] = newEntry;
}

string HashTable::search(const string& key) const {
    int idx = hashFunction(key);
    HashEntry* curr = buckets[idx];
    while (curr != nullptr) {
        if (curr->key == key) {
            return curr->value;
        }
        curr = curr->next;
    }
    return "Not Found";
}

bool HashTable::remove(const string& key) {
    int idx = hashFunction(key);
    HashEntry* curr = buckets[idx];
    HashEntry* prev = nullptr;

    while (curr != nullptr) {
        if (curr->key == key) {
            if (prev == nullptr) {
                buckets[idx] = curr->next;
            } else {
                prev->next = curr->next;
            }
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

void HashTable::display() const {
    cout << "\n--- Ambulance / Resource Hash Table ---\n";
    for (int i = 0; i < TABLE_SIZE; ++i) {
        if (buckets[i] != nullptr) {
            cout << "Bucket [" << i << "]: ";
            HashEntry* curr = buckets[i];
            while (curr != nullptr) {
                cout << "(" << curr->key << " -> " << curr->value << ")";
                if (curr->next != nullptr) cout << " -> ";
                curr = curr->next;
            }
            cout << "\n";
        }
    }
}

LinkedList::LinkedList() : head(nullptr), tail(nullptr) {}

LinkedList::~LinkedList() {
    DispatchLogNode* curr = head;
    while (curr != nullptr) {
        DispatchLogNode* nextNode = curr->next;
        delete curr;
        curr = nextNode;
    }
    head = tail = nullptr;
}

void LinkedList::append(const string& entry) {
    DispatchLogNode* newNode = new DispatchLogNode(entry);
    if (tail == nullptr) {
        head = tail = newNode;
        return;
    }
    tail->next = newNode;
    newNode->prev = tail;
    tail = newNode;
}

void LinkedList::deleteLast() {
    if (tail == nullptr) return;
    DispatchLogNode* temp = tail;
    if (head == tail) {
        head = tail = nullptr;
    } else {
        tail = tail->prev;
        tail->next = nullptr;
    }
    delete temp;
}

void LinkedList::displayForward() const {
    cout << "\n--- Dispatch Logs (Oldest -> Latest) ---\n";
    DispatchLogNode* curr = head;
    while (curr != nullptr) {
        cout << "  * " << curr->logEntry << "\n";
        curr = curr->next;
    }
}

void LinkedList::displayBackward() const {
    cout << "\n--- Dispatch Logs (Latest -> Oldest) ---\n";
    DispatchLogNode* curr = tail;
    while (curr != nullptr) {
        cout << "  * " << curr->logEntry << "\n";
        curr = curr->prev;
    }
}

bool LinkedList::isEmpty() const {
    return head == nullptr;
}