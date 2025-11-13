#include "StrUtil.hpp"

#ifdef __linux__
#include <cstring>
#endif

#include <sstream>

using namespace mpc;

std::string StrUtil::replaceAll(const std::string &str, char c,
                                const std::string &replacement)
{
    if (replacement.length() > 1)
    {
        return "<error>";
    }
    std::string res;
    for (char ch : str)
    {
        if (ch == c)
        {
            if (replacement.length() == 0 || replacement == "")
            {
                continue;
            }
            res.push_back(replacement[0]);
        }
        else
        {
            res.push_back(ch);
        }
    }
    return res;
}

bool StrUtil::eqIgnoreCase(const std::string &s1, const std::string &s2)
{
    if (s1.length() != s2.length())
    {
        return false;
    }
    for (int i = 0; i < s1.length(); i++)
    {
        if (s1[i] != tolower(s2[i]) && s1[i] != toupper(s2[i]))
        {
            return false;
        }
    }
    return true;
}

std::string StrUtil::toUpper(const std::string &str)
{
    std::string res = str;
    for (int i = 0; i < res.length(); i++)
    {
        res[i] = toupper(res[i]);
    }
    return res;
}

std::string StrUtil::toLower(const std::string &str)
{
    std::string res = str;

    for (int i = 0; i < res.length(); i++)
    {
        res[i] = tolower(res[i]);
    }

    return res;
}

std::string StrUtil::padLeft(std::string str, const std::string &pad, int size)
{
    if (str.length() >= size)
    {
        return str;
    }

    std::string result = "";

    for (int i = 0; i < size - str.size(); i++)
    {
        result.append(pad);
    }

    int counter = 0;

    for (int i = size - static_cast<int>(str.size()); i < size; i++)
    {
        result.append(str.substr(counter++, 1));
    }

    return result;
};

std::string StrUtil::padRight(std::string str, const std::string &pad, int size)
{
    if (str.length() >= size)
    {
        return str;
    }

    std::string result = "";

    for (int i = 0; i < str.size(); i++)
    {
        result.append(str.substr(i, 1));
    }

    for (int i = static_cast<int>(str.size()); i < size; i++)
    {
        result.append(pad);
    }

    return result;
}

std::string StrUtil::trim(const std::string &str)
{
    std::string copy = str;
    while (!copy.empty() && isspace(copy.back()))
    {
        copy.pop_back();
    }
    return copy;
}

std::string StrUtil::TrimDecimals(const std::string &str, int count)
{
    try
    {
        // We check if we're dealing with a float.
        stof(str);

        std::string copy = str;
        const int index = static_cast<int>(str.find("."));
        const int numberOfDecimalsFound =
            static_cast<int>(str.length()) - index + 1;
        if (numberOfDecimalsFound <= count)
        {
            // Trimming is not possible
            return str;
        }
        copy = copy.substr(0, index + 1 + count);
        return copy;
    }
    catch (const std::exception &)
    {
        // In this case we're fine returning str
    }
    return str;
}

std::string StrUtil::TrimDecimals(double d, int count)
{
    return TrimDecimals(std::to_string(d), count);
}

bool StrUtil::hasEnding(std::string const &fullString,
                        std::string const &ending)
{
    if (fullString.length() >= ending.length())
    {
        return 0 == fullString.compare(fullString.length() - ending.length(),
                                       ending.length(), ending);
    }
    else
    {
        return false;
    }
}

inline std::string ltrim(const std::string &s, char c)
{
    size_t start = s.find_first_not_of(c);
    return start == std::string::npos ? "" : s.substr(start);
}

inline std::string rtrim(const std::string &s, char c)
{
    size_t end = s.find_last_not_of(c);
    return end == std::string::npos ? "" : s.substr(0, end + 1);
}

inline std::string trim_by_c(const std::string &s, char c)
{
    return ltrim(rtrim(s, c), c);
}

std::vector<std::string> StrUtil::split(const std::string &s, char c)
{
    auto str = trim_by_c(s, c);
    std::stringstream ss = std::stringstream(str);
    std::string segment;
    std::vector<std::string> seglist;

    while (getline(ss, segment, c))
    {
        seglist.push_back(segment);
    }

    return seglist;
}
