#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <string>
#include <vector>

struct HospitalInfo {
    std::string id;
    std::string name;
    int zoneId;
    int availableBeds;
    int icuUnits;
};

class HospitalManager {
private:
    std::vector<HospitalInfo> hospitals;

public:
    void addHospital(const HospitalInfo& h);
    void listHospitals() const;
    void sortHospitalsByBeds();
    int findNearestHospitalZone(int emergencyZone) const;
};

#endif