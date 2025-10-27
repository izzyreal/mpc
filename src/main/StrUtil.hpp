#pragma once
#include <string>
#include <vector>

namespace mpc
{
    class StrUtil
    {
    public:
        static std::string padLeft(std::string str, std::string pad, int size);
        static std::string padRight(std::string str, std::string pad, int size);
        static std::string trim(const std::string &str);
        static std::string toUpper(std::string str);
        static std::string toLower(std::string str);
        static bool eqIgnoreCase(std::string s1, std::string s2);
        static std::string replaceAll(std::string str, char c,
                                      std::string replacement);
        static std::string TrimDecimals(const std::string &str,
                                        int decimalCount);
        static std::string TrimDecimals(double d, int decimalCount);

        static bool hasEnding(std::string const &fullString,
                              std::string const &ending);

        static std::vector<std::string> split(const std::string &s, char c);
    };
} // namespace mpc
