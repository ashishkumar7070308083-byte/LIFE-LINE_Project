#include "MinHeap.h"
#include "Graph.h"
#include <climits>
#include <stdexcept>

using namespace std;

void MinHeap::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[index].first < heap[parent].first) {
            swap(heap[index], heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

void MinHeap::heapifyDown(int index) {
    int n = static_cast<int>(heap.size());
    while (true) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < n && heap[left].first < heap[smallest].first) {
            smallest = left;
        }
        if (right < n && heap[right].first < heap[smallest].first) {
            smallest = right;
        }
        if (smallest != index) {
            swap(heap[index], heap[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

void MinHeap::push(int priority, int value) {
    heap.push_back({priority, value});
    heapifyUp(static_cast<int>(heap.size()) - 1);
}

pair<int, int> MinHeap::pop() {
    if (heap.empty()) {
        throw runtime_error("MinHeap is empty: cannot pop");
    }
    pair<int, int> topItem = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    if (!heap.empty()) {
        heapifyDown(0);
    }
    return topItem;
}

pair<int, int> MinHeap::top() const {
    if (heap.empty()) {
        throw runtime_error("MinHeap is empty: cannot peek");
    }
    return heap[0];
}

bool MinHeap::isEmpty() const {
    return heap.empty();
}

int MinHeap::size() const {
    return static_cast<int>(heap.size());
}