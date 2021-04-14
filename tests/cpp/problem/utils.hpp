#pragma once

#include "ioh.hpp"

inline fs::path find_test_file(const std::string &filename)
{
    auto file = fs::path("IOHexperimenter") / fs::path("tests")
        / fs::path("cpp") / fs::path("problem") / filename;

    fs::path root;
    for (const auto &e : fs::current_path())
    {
        root /= e;
        if (exists(root / file))
        {
            file = root / file;
            break;
        }
    }
    return file;
}


inline std::vector<std::string> split(const std::string &str, const std::string &pattern)
{
    std::vector<std::string> res;
    if (str.empty())
        return res;

    auto search_string = str + pattern;
    auto pos = search_string.find(pattern);

    while (pos != search_string.npos)
    {
        res.push_back(search_string.substr(0, pos));
        search_string = search_string.substr(pos + 1, search_string.size());
        pos = search_string.find(pattern);
    }
    return res;
}

inline std::vector<double> string_to_vector_double(const std::string &s)
{
    std::vector<double> x;
    size_t i = 0;

    auto tmp = split(s, ",");
    while (i != tmp.size())
    {
        x.push_back(stod(tmp[i]));
        i++;
    }
    return x;
}

inline std::vector<int> string_to_vector_int(const std::string &s)
{
    std::vector<int> x;
    size_t i = 0;
    while (i != s.size())
    {
        x.push_back(s[i] - '0');
        i++;
    }
    return x;
}
