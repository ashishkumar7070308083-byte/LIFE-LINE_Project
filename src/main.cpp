#include "Graph.h"
#include "MinHeap.h"
#include "PriorityQueue.h"
#include "HashTable.h"
#include "LinkedList.h"
#include "Hospital.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

namespace {

void loadCitiesAndRoads(const string& filePath, Graph& cityGraph) {
    ifstream inFile(filePath);
    if (!inFile.is_open()) {
        cout << "[Warning] Could not open " << filePath << ". Using default roads.\n";
        cityGraph.addEdge(0, 1, 5);
        cityGraph.addEdge(0, 2, 6);
        cityGraph.addEdge(2, 1, 3);
        cityGraph.addEdge(1, 3, 7);
        cityGraph.addEdge(2, 3, 9);
        cityGraph.addEdge(3, 4, 6);
        return;
    }

    int totalCities = 0;
    if (inFile >> totalCities && totalCities > 0) {
        int u, v, w;
        while (inFile >> u >> v >> w) {
            cityGraph.addEdge(u, v, w);
        }
    }
}

void loadAmbulances(const string& filePath, HashTable& ambulanceTable) {
    ifstream inFile(filePath);
    if (!inFile.is_open()) {
        cout << "[Warning] Could not open " << filePath << ". Loading fallback ambulance.\n";
        ambulanceTable.insert("AMB-CH01", "Driver: Gurpreet_Singh | Zone: 0 | Status: Available");
        return;
    }

    string id, driver;
    int zoneId, available;
    while (inFile >> id >> driver >> zoneId >> available) {
        string status = (available == 1) ? "Available" : "Dispatched";
        string details = "Driver: " + driver + " | Zone: " + to_string(zoneId) + " | Status: " + status;
        ambulanceTable.insert(id, details);
    }
}

void loadEmergencies(const string& filePath, PriorityQueue& triageQueue) {
    ifstream inFile(filePath);
    if (!inFile.is_open()) {
        cout << "[Warning] Could not open " << filePath << ". Loading fallback emergency.\n";
        triageQueue.enqueue({"INC-1", 10, "Amandeep_Kaur", 4, "Cardiac_Arrest"});
        return;
    }

    string id, name, issue;
    int severity, zoneId;
    while (inFile >> id >> severity >> name >> zoneId >> issue) {
        triageQueue.enqueue({id, severity, name, zoneId, issue});
    }
}

} // namespace

int main() {
    cout << "============================================================\n";
    cout << "   LIFE-LINE: INTELLIGENT EMERGENCY & RESOURCE SYSTEM       \n";
    cout << "   Chandigarh - Mohali - CGC Corridor                      \n";
    cout << "============================================================\n";

    Graph cityGraph(5);
    cityGraph.setCityName(0, "CGC Trauma Hub (Landran)");
    cityGraph.setCityName(1, "Mohali Phase-7");
    cityGraph.setCityName(2, "Kharar Junction");
    cityGraph.setCityName(3, "PGI / Sector-12 Chd");
    cityGraph.setCityName(4, "ISBT Sector-17 Chd");

    loadCitiesAndRoads("data/cities.txt", cityGraph);
    cityGraph.printGraph();

    HashTable ambulanceTable;
    loadAmbulances("data/ambulances.txt", ambulanceTable);
    ambulanceTable.display();
    cout << "\n[Hash Lookup -> AMB-CH01]: " << ambulanceTable.search("AMB-CH01") << "\n";

    HospitalManager hospitalMgr;
    hospitalMgr.addHospital({"HOSP-CH1", "PGI Sector-12 Trauma Center", 3, 36, 12});
    hospitalMgr.addHospital({"HOSP-CH2", "CGC Emergency Care Unit", 0, 22, 6});
    hospitalMgr.addHospital({"HOSP-CH3", "Mohali Civil & Care Hospital", 1, 18, 5});
    hospitalMgr.sortHospitalsByBeds();
    hospitalMgr.listHospitals();

    PriorityQueue triageQueue;
    LinkedList dispatchLogs;
    loadEmergencies("data/emergencies.txt", triageQueue);

    int hospitalZone = 0;
    while (!triageQueue.isEmpty()) {
        EmergencyCase topCase = triageQueue.dequeue();
        cout << "\n------------------------------------------------------------\n";
        cout << "[HIGHEST SEVERITY EMERGENCY] ID: " << topCase.id
             << " | Severity: " << topCase.severity << "/10"
             << " | Patient: " << topCase.patientName
             << " | Issue: " << topCase.issue
             << " | Zone: " << cityGraph.getCityName(topCase.zoneId) << "\n";

        cityGraph.shortestPathDijkstra(hospitalZone, topCase.zoneId);

        string logEntry = topCase.id + " (" + topCase.patientName + ", Sev " +
                          to_string(topCase.severity) + ") -> Dispatched from " +
                          cityGraph.getCityName(hospitalZone) + " to " +
                          cityGraph.getCityName(topCase.zoneId);
        dispatchLogs.append(logEntry);
    }

    cityGraph.mstPrim();
    dispatchLogs.displayForward();

    cout << "\n[Undo Last Log Entry...]\n";
    dispatchLogs.deleteLast();
    dispatchLogs.displayBackward();

    ofstream outJson("frontend/state.json");
    if (outJson.is_open()) {
        outJson << "{\n";
        outJson << "  \"system\": \"LIFE-LINE\",\n";
        outJson << "  \"corridor\": \"Chandigarh - Mohali - CGC\",\n";
        outJson << "  \"lookup_sample\": \"" << ambulanceTable.search("AMB-CH01") << "\",\n";
        outJson << "  \"status\": \"Synced with C++ execution\"\n";
        outJson << "}\n";
        outJson.close();
        cout << "\n[Exported frontend/state.json successfully]\n";
    }

    cout << "\n============================================================\n";
    cout << "   LIFE-LINE EXECUTION COMPLETED SUCCESSFULLY               \n";
    cout << "============================================================\n";
    return 0;
}