#pragma once
#include <string>
#include <vector>

namespace mpc
{
    class StrUtil
    {
    public:
        static std::string padLeft(std::string str, const std::string &pad,
                                   int size);
        static std::string padRight(std::string str, const std::string &pad,
                                    int size);
        static std::string trim(const std::string &str);
        static std::string toUpper(const std::string &str);
        static std::string toLower(const std::string &str);
        static bool eqIgnoreCase(const std::string &s1, const std::string &s2);
        static std::string replaceAll(const std::string &str, char c,
                                      const std::string &replacement);
        static std::string TrimDecimals(const std::string &str,
                                        int decimalCount);
        static std::string TrimDecimals(double d, int decimalCount);

        static bool hasEnding(std::string const &fullString,
                              std::string const &ending);

        static std::vector<std::string> split(const std::string &s, char c);
    };
} // namespace mpc
