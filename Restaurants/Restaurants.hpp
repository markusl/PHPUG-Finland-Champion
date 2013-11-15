#pragma once
#include <string>
#include <vector>

struct OpeningHours {
    std::string Restaurant; size_t Hours;

    const bool operator < (const OpeningHours &r) const {
        return (Hours > r.Hours); // Sort this way
    }
    const bool operator ==(const OpeningHours &r) const {
        return Restaurant == r.Restaurant && Hours == r.Hours;
    }
    static OpeningHours construct(const std::string &name, const size_t hours)
    {
        return{ name, hours };
    }
};

size_t count_hours_single_timespan(const std::string &opening_times);
std::vector<OpeningHours> restaurants_sorted(const std::vector<std::vector<std::string>> &contents);

