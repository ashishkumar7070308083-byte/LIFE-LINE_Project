#include "PriorityQueue.h"
#include <stdexcept>
#include <utility>

using namespace std;

void PriorityQueue::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (pq[index].severity > pq[parent].severity) {
            swap(pq[index], pq[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

void PriorityQueue::heapifyDown(int index) {
    int n = static_cast<int>(pq.size());
    while (true) {
        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < n && pq[left].severity > pq[largest].severity) {
            largest = left;
        }
        if (right < n && pq[right].severity > pq[largest].severity) {
            largest = right;
        }
        if (largest != index) {
            swap(pq[index], pq[largest]);
            index = largest;
        } else {
            break;
        }
    }
}

void PriorityQueue::enqueue(const EmergencyCase& item) {
    pq.push_back(item);
    heapifyUp(static_cast<int>(pq.size()) - 1);
}

EmergencyCase PriorityQueue::dequeue() {
    if (pq.empty()) {
        throw runtime_error("PriorityQueue is empty: cannot dequeue");
    }
    EmergencyCase topCase = pq[0];
    pq[0] = pq.back();
    pq.pop_back();
    if (!pq.empty()) {
        heapifyDown(0);
    }
    return topCase;
}

EmergencyCase PriorityQueue::peek() const {
    if (pq.empty()) {
        throw runtime_error("PriorityQueue is empty: cannot peek");
    }
    return pq[0];
}

bool PriorityQueue::isEmpty() const {
    return pq.empty();
}

int PriorityQueue::size() const {
    return static_cast<int>(pq.size());
}