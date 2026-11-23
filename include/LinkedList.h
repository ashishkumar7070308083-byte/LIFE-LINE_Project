#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <string>

struct DispatchLogNode {
    std::string logEntry;
    DispatchLogNode* prev;
    DispatchLogNode* next;
    explicit DispatchLogNode(const std::string& entry) : logEntry(entry), prev(nullptr), next(nullptr) {}
};

class LinkedList {
private:
    DispatchLogNode* head;
    DispatchLogNode* tail;

public:
    LinkedList();
    ~LinkedList();
    void append(const std::string& entry);
    void deleteLast();
    void displayForward() const;
    void displayBackward() const;
    bool isEmpty() const;
};

#endif