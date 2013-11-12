/// Parts borrowed from Stack Overflow or other internet forums
namespace lib
{

    // Definitions
    static const char Colon = ':';
    static const char Semicolon = ';';
    static const char Comma = ',';
    static const char Space = ' ';
    static const char Dash = '-';

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

inline std::string trim_start(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

template<class Fn>
auto map_file_contents(const std::string &file_name, Fn f) -> std::vector<decltype(f(std::string()))>
{
    std::vector<decltype(f(std::string()))> values;
    std::ifstream fin(file_name);
    for(std::string line; std::getline(fin, line);)
    {
        values.push_back(f(line));
    }
    return values;
};

/// Parallel map
template<typename Ty, typename F>
auto pmap(std::vector<Ty> vector, F func) -> std::vector<decltype(func(Ty()))>
{
    std::vector<decltype(func(Ty()))> result(vector.size());
    concurrency::parallel_transform(vector.begin(), vector.end(),
        begin(result), func);
    return result;
}

template<typename Ty, typename F>
auto map(std::vector<Ty> vector, F func) -> std::vector<decltype(func(Ty()))>
{
    std::vector<decltype(func(Ty()))> result(vector.size());
    std::transform(vector.begin(), vector.end(), begin(result), func);
    return result;
}

template<typename T1, typename T2, typename F>
auto zip(const std::vector<T1>& first, const std::vector<T2>& second, F func) -> std::vector<decltype(func(T1(), T2()))>
{
    assert(first.size() == second.size());
    std::vector<decltype(func(T1(), T2()))> result(first.size());
    std::transform(first.begin(), first.end(), second.begin(),
        result.begin(), func);
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

} // namespace lib
