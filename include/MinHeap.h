#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <vector>
#include <utility>

class MinHeap {
private:
    std::vector<std::pair<int, int>> heap;
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    void push(int priority, int value);
    std::pair<int, int> pop();
    std::pair<int, int> top() const;
    bool isEmpty() const;
    int size() const;
};

#endif