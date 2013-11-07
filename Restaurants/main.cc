// Written by Markus Lindqvist, 
/*
 Implement a PHP application that reads the given ravintolat.csv file.
 The program should analyze the input file and print the following :
 The name of the restaurant that is open the most and the amount of hours it’s open
 The name of the restaurant that is open the least and the amount of hours it’s open
*/

#include <functional>
#include <numeric>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <locale>
#include <cctype>
#include <assert.h>
//#define CONCURRENCY
#if CONCURRENCY
#include <ppl.h>
#endif
#include "library.hpp"

static const std::string Ja{ " ja " };

static const std::map<std::string, int> DayMap{
    { "Ma", 1 }, { "Ti", 2 }, { "Ke", 3 }, { "To", 4 },
    { "Pe", 5 }, { "La", 6 }, { "Su", 7 } };

/** Converts restaurant opening times from string representation to number of hours
 * @param opening_times     The opening times. Eg. "Ma-Pe 10:00-12:00 ja 12:30-14:30".
 * @return                  The opening hours total, in full hours.
 */
size_t count_hours_single_timespan(const std::string &opening_times)
{
    // Possible inputs: "Ma-Pe 10:00-16:00", or "To 09:00-16:30", "Ma-Pe 10:00-12:00 ja 12:30-14:30"
    const auto full_time = lib::trim_start(std::string(opening_times));
    const size_t pos = full_time.find(' ');

    // Extract the hour parts from string: "10:00-12:00 ja 12:30-14:30" -> ["10:00-12:00", "12:30-14:30"]
    const auto hour_parts = [](const std::string &hours) -> std::vector<std::string>
    {
        const size_t two_parts = hours.find(Ja);
        if(std::string::npos == two_parts)
            return{ hours };
        else
            return{ hours.substr(0, two_parts), hours.substr(two_parts + Ja.length()) };
    }(full_time.substr(pos));

    // Convert hour span from string to number of hours: "10:00-12:00" -> 2
    const auto count_hours = [](const std::string &hours) -> size_t {
        const auto sum_minutes = [](std::vector<std::string> elem)
        {
            if(elem.size() >= 2)
                return std::stoi(elem[0]) * 60 + std::stoi(elem[1]);
            return 0;
        };
        const auto times_split = lib::map(lib::split_by<lib::Colon>,
                                          lib::split_by<lib::Dash>(hours));
        const auto times = lib::map(sum_minutes, times_split);
        return (times[1] - times[0]) / 60;
    };

    // Sum the hour parts: ["10:00-12:00", "12:30-14:30"] -> 4
    const auto hours = lib::sum_vector(lib::map(count_hours, hour_parts));

    // Contains the day span string "Ma-Pe"
    const std::string days_str = full_time.substr(0, pos);

    // Convert day span to number of days: "Ma-Pe" -> 5
    const auto days = [&](const std::string &days)
    {
        const size_t dash = days.find(lib::Dash);
        if(std::string::npos == dash)
            return 1;

        const auto end = days.substr(dash + 1);
        const auto start = days.substr(0, dash);
        return DayMap.find(end)->second - DayMap.find(start)->second + 1;
    }(days_str);

    return hours * days;
}

/** From the rows and columns in the data, extract restaurant opening hours, sorted in descending order. */
std::vector<std::pair<std::string, size_t>> restaurants_sorted(const std::vector<std::vector<std::string>> &contents)
{
    const auto count_hours = [](const std::vector<std::string> &hours)
    {
        return lib::map(count_hours_single_timespan, hours);
    };

    const auto restaurant_column = lib::map(lib::select_nth<1, std::string>, contents);
    // ["Ma-Pe 10:00-16:00, To 09:00-16:30", ...]
    const auto time_column = lib::map(lib::select_nth<4, std::string>, contents);
    // [["Ma-Pe 10:00-16:00", "To 09:00-16:30"], [...], ...]
    const auto times = lib::map(lib::split_by<lib::Comma>, time_column);
    // [["10:00-16:00", "09:00-16:30"], ...] -> [[6, 7], ...]
    const auto hours = lib::map(count_hours, times);
    // [[6, 7], ...] -> [13, ...]
    const auto hours_summed = lib::map(lib::sum_vector, hours);

    auto combined = lib::zip([](const std::string &name, const size_t hours) { return std::make_pair(name, hours); }, restaurant_column, hours_summed);
    std::sort(combined.begin(), combined.end(), lib::by_second<std::pair<std::string, size_t>>);

    return combined;
}

void unit_tests()
{
    assert(count_hours_single_timespan("Ma-Pe 10:00-16:00") == 5 * 6);
    assert(count_hours_single_timespan("To 09:00-16:30") == 7);
    assert(count_hours_single_timespan("Pe 09:30-16:00") == 6);
    assert(count_hours_single_timespan("Ke-Pe 10:00-16:00") == 3 * 6);
    assert(count_hours_single_timespan("Ma-Pe 10:00-12:00 ja 12:30-14:30") == 5 * 2 + 5 * 2);
}

int main(const int argc, char **argv)
{
#if _DEBUG
    unit_tests();
#endif

    // [line, ...]
    const auto contents = lib::map_file_contents(std::string(argv[1]), lib::split_by<lib::Semicolon>);
    const auto restaurants = restaurants_sorted(contents);

    std::cout << restaurants[0].first << " #1 open " << restaurants[1].second << " hours per week\n";
    std::cout << restaurants[restaurants.size() - 1].first << " #2 open " << restaurants[restaurants.size() - 1].second << " hours per week" << std::endl;
}
