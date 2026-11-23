#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <vector>
#include <string>

struct EmergencyCase {
    std::string id;
    int severity;
    std::string patientName;
    int zoneId;
    std::string issue;
};

class PriorityQueue {
private:
    std::vector<EmergencyCase> pq;
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    void enqueue(const EmergencyCase& item);
    EmergencyCase dequeue();
    EmergencyCase peek() const;
    bool isEmpty() const;
    int size() const;
};

#endif