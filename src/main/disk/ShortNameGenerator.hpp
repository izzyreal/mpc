#pragma once

#include <vector>
#include <string>

namespace mpc::disk
{
    class ShortName;
}

namespace mpc::disk
{
    class ShortNameGenerator
    {

        std::vector<std::string> usedNames;

    public:
        static bool validChar(char toTest);
        static bool isSkipChar(char c);

    private:
        bool cleanString(const std::string &s) const;
        std::string stripLeadingPeriods(const std::string &str) const;

    public:
        ShortName generateShortName(const std::string &longFullName);
        static std::string tidyString(const std::string &dirty);

        ShortNameGenerator(const std::vector<std::string> &usedNames);
    };
} // namespace mpc::disk
