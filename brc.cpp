#include "brc.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace std;

int64_t tempFromString(string_view s) {
    int64_t result = 0;
    int64_t sign = 1;
    for (char c : s) {
        if ('0' <= c && c <= '9') {
            result *= 10;
            result += c - '0';
        } else if (c == '.') {
            continue;
        } else if (c == '-') {
            sign = -1;
        } else {
            throw runtime_error("give me a number idiot");
        }
    }
    return sign * result;
}

Station::Station()
    : minTemp(INT64_MAX), maxTemp(INT64_MIN), sumTemp(0), count(0) {}

void Station::add(int64_t entry) {
    minTemp = min(minTemp, entry);
    maxTemp = max(maxTemp, entry);
    sumTemp += entry;
    count++;
}

void Station::add(const Station &other) {
    minTemp = min(minTemp, other.minTemp);
    maxTemp = max(maxTemp, other.maxTemp);
    sumTemp += other.sumTemp;
    count += other.count;
}

ostream &operator<<(ostream &os, const Station &station) {
    int64_t avg = int64_t(station.sumTemp / double(station.count));
    if (station.minTemp < 0) {
        os << '-';
    }
    os << abs(station.minTemp) / 10 << '.' << abs(station.minTemp) % 10 << '/';
    if (avg < 0) {
        os << '-';
    }
    os << abs(avg) / 10 << '.' << abs(avg) % 10 << '/';
    if (station.maxTemp < 0) {
        os << '-';
    }
    os << abs(station.maxTemp) / 10 << '.' << abs(station.maxTemp) % 10;
    return os;
}

void StationManager::add(string_view stationName, int64_t temp) {
    auto entry = stations.find(stationName);
    if (entry != stations.end()) {
        entry->second->add(temp);
    } else {
        auto station = new Station();
        station->add(temp);
        stations[string(stationName)] = unique_ptr<Station>(station);
    }
}

void StationManager::add(string_view stationName, const Station *station) {
    auto entry = stations.find(stationName);
    if (entry != stations.end()) {
        entry->second->add(*station);
    } else {
        stations[string(stationName)] =
            unique_ptr<Station>(new Station(*station));
    }
}

void StationManager::add(const StationManager &other) {
    for (auto const &[key, val] : other.stations) {
        this->add(key, val.get());
    }
}

ostream &operator<<(ostream &os, const StationManager &m) {
    vector<pair<string_view, const Station *>> vec;
    for (auto const &[key, val] : m.stations) {
        vec.push_back(make_pair(string_view(key), val.get()));
    }
    sort(vec.begin(), vec.end(),
         [](auto &left, auto &right) { return left.first < right.first; });
    os << '{';
    for (size_t i = 0; i < vec.size(); i++) {
        os << vec[i].first << '=' << *(vec[i].second);
        if (i < vec.size() - 1) {
            os << ", ";
        }
    }
    os << '}';
    return os;
}
