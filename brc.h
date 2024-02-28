#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>

int64_t tempFromString(std::string_view s);

class Station {
  public:
    Station();
    void add(int64_t entry);
    void add(const Station &other);

  private:
    int64_t minTemp;
    int64_t maxTemp;
    int64_t sumTemp;
    int64_t count;

    friend std::ostream &operator<<(std::ostream &os, const Station &station);
};

struct string_hash {
    using hash_type = std::hash<std::string_view>;
    using is_transparent = void;

    std::size_t operator()(const char *str) const { return hash_type{}(str); }
    std::size_t operator()(std::string_view str) const {
        return hash_type{}(str);
    }
    std::size_t operator()(std::string const &str) const {
        return hash_type{}(str);
    }
};

class StationManager {
  public:
    StationManager() {}
    StationManager(const StationManager &) = delete;
    StationManager &operator=(const StationManager &) = delete;
    void add(std::string_view stationName, int64_t temp);
    void add(std::string_view stationName, const Station *station);
    void add(const StationManager &other);

  private:
    std::unordered_map<std::string, std::unique_ptr<Station>, string_hash,
                       std::equal_to<>>
        stations;
    friend std::ostream &operator<<(std::ostream &os, const StationManager &m);
};
