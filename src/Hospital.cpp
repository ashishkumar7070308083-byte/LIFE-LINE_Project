#include "Hospital.h"
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

void HospitalManager::addHospital(const HospitalInfo& h) {
    hospitals.push_back(h);
}

void HospitalManager::listHospitals() const {
    cout << "\n--- Registered Hospitals ---\n";
    for (const auto& h : hospitals) {
        cout << "  [" << h.id << "] " << h.name
             << " | Zone: " << h.zoneId
             << " | Beds: " << h.availableBeds
             << " | ICU: " << h.icuUnits << "\n";
    }
}

void HospitalManager::sortHospitalsByBeds() {
    sort(hospitals.begin(), hospitals.end(), [](const HospitalInfo& a, const HospitalInfo& b) {
        return a.availableBeds > b.availableBeds;
    });
}

int HospitalManager::findNearestHospitalZone(int emergencyZone) const {
    if (hospitals.empty()) return -1;

    int bestZone = hospitals[0].zoneId;
    int minDiff = abs(hospitals[0].zoneId - emergencyZone);

    for (size_t i = 1; i < hospitals.size(); ++i) {
        int diff = abs(hospitals[i].zoneId - emergencyZone);
        if (diff < minDiff) {
            minDiff = diff;
            bestZone = hospitals[i].zoneId;
        }
    }
    return bestZone;
}