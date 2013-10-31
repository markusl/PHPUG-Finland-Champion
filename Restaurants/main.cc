// Written by Markus Lindqvist, some code snippes found from Stack Overflow or other internet forums
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
#include <vector>
#include <map>
#include <locale>
#include <cctype>
#include <assert.h>

// Definitions
static const char Colon = ':';
static const char Semicolon = ';';
static const char Comma = ',';
static const char Space = ' ';
static const char Dash = '-';
static const std::string Ja{ " ja " };

static const std::map<std::string, int> DayMap{
    { "Ma", 1 }, { "Ti", 2 }, { "Ke", 3 }, { "To", 4 },
    { "Pe", 5 }, { "La", 6 }, { "Su", 7 } };

// "Library functions"
template<char Separator>
struct tokens : std::ctype<char>
{
    tokens() : std::ctype<char>(get_table()) {}

    static std::ctype_base::mask const* get_table()
    {
        typedef std::ctype<char> cctype;
        static const cctype::mask *const_rc = cctype::classic_table();

        static cctype::mask rc[cctype::table_size];
        std::memcpy(rc, const_rc, cctype::table_size * sizeof(cctype::mask));

        rc[Separator] = std::ctype_base::space;
        rc[Space] = std::ctype_base::alnum;
        return &rc[0];
    }
};

template<char Separator>
std::vector<std::string> split_by(const std::string &line)
{
    std::istringstream in(line);
    in.imbue(std::locale(std::locale(), new tokens<Separator>()));
    return std::vector<std::string>(std::istream_iterator<std::string>(in),
        std::istream_iterator<std::string>());
}

inline std::string &trim_start(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

template<class Ty>
std::vector<std::vector<Ty>> file_contents(const std::string &file_name)
{
    std::vector<std::vector<Ty>> values;
    std::ifstream fin(file_name);
    for(std::string line; std::getline(fin, line);)
    {
        values.push_back(split_by<';'>(line));
    }
    return values;
};

template<typename Ty, typename F>
auto map(F func, const std::vector<Ty> &vector) -> std::vector<decltype(func(Ty()))>
{
    std::vector<decltype(func(Ty()))> result;
    std::transform(vector.begin(), vector.end(),
        std::back_inserter(result), func);
    return result;
}

template<typename T1, typename T2, typename F>
auto zip(F func, const std::vector<T1>& first, const std::vector<T2>& second) -> std::vector<decltype(func(T1(), T2()))>
{
    std::vector<decltype(func(T1(), T2()))> result;
    std::transform(first.begin(), first.end(), second.begin(),
        std::back_inserter(result), func);
    return result;
}

size_t sum_vector(const std::vector<size_t> &to_sum)
{
    return std::accumulate(to_sum.begin(), to_sum.end(), 0);
}

template<int Column, class Ty>
auto select_nth(const std::vector<Ty> &row) -> Ty
{
    return row[Column];
}

template<class Ty>
bool by_second(const Ty &t1, const Ty &t2) {
    return t1.second > t2.second;
}

// Start user code

/** Converts restaurant opening times from string representation to number of hours
 * @param opening_times     The opening times. Eg. "Ma-Pe 10:00-12:00 ja 12:30-14:30".
 * @return                  The opening hours total, in full hours.
 */
size_t count_hours_single_timespan(const std::string &opening_times)
{
    // Possible inputs: "Ma-Pe 10:00-16:00", or "To 09:00-16:30", "Ma-Pe 10:00-12:00 ja 12:30-14:30"
    const auto full_time = trim_start(std::string(opening_times));
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
        const auto sum_minutes = [](const std::vector<std::string> &elem)
        {
            return std::stoi(elem[0]) * 60 + std::stoi(elem[1]);
        };
        const auto times_split = map(split_by<Colon>, split_by<Dash>(hours));
        const auto times = map(sum_minutes, times_split);
        return (times[1] - times[0]) / 60;
    };

    // Sum the hour parts: ["10:00-12:00", "12:30-14:30"] -> 4
    const auto hours = sum_vector(map(count_hours, hour_parts));

    // Contains the day span string "Ma-Pe"
    const std::string days_str = full_time.substr(0, pos);

    // Convert day span to number of days: "Ma-Pe" -> 5
    const auto days = [&](const std::string &days)
    {
        const auto dash = days.find(Dash);
        if(std::string::npos == dash)
            return 1;

        std::string end = days.substr(dash + 1);
        std::string start = days.substr(0, dash);
        return DayMap.find(end)->second - DayMap.find(start)->second + 1;
    }(days_str);

    return hours * days;
}

/** From the rows and columns in the data, extract restaurant opening hours, sorted in descending order. */
std::vector<std::pair<std::string, size_t>> restaurants_sorted(const std::vector<std::vector<std::string>> &contents)
{
    const auto count_hours = [](const std::vector<std::string> &hours)
    {
        return map(count_hours_single_timespan, hours);
    };

    const auto restaurant_column = map(select_nth<1, std::string>, contents);
    // ["Ma-Pe 10:00-16:00, To 09:00-16:30", ...]
    const auto time_column = map(select_nth<4, std::string>, contents);
    // [["Ma-Pe 10:00-16:00", "To 09:00-16:30"], [...], ...]
    const auto times = map(split_by<Comma>, time_column);
    // [["10:00-16:00", "09:00-16:30"], ...] -> [[6, 7], ...]
    const auto hours = map(count_hours, times);
    // [[6, 7], ...] -> [13, ...]
    const auto hours_summed = map(sum_vector, hours);

    auto combined = zip([](const std::string &name, const size_t hours) { return std::make_pair(name, hours); }, restaurant_column, hours_summed);
    std::sort(combined.begin(), combined.end(), by_second<std::pair<std::string, size_t>>);

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
    const auto contents = file_contents<std::string>(argv[1]);
    const auto restaurants = restaurants_sorted(contents);

    std::cout << restaurants[0].first << " #1 open " << restaurants[1].second << " hours per week\n";
    std::cout << restaurants[restaurants.size() - 1].first << " #2 open " << restaurants[restaurants.size() - 1].second << " hours per week" << std::endl;
}
