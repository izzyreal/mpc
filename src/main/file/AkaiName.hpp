#pragma once

#include <string>
#include <vector>

namespace mpc::file
{
    class AkaiName
    {
    private:
        static std::vector<char> allowedSpecialCharacters;

    private:
        static bool isCharThatShouldBeRemoved(const char &c);

    private:
        static std::string cleanUpName(const std::string &name);
        static std::string removeLeadingPeriod(const std::string &str);

    public:
        static bool isAkaiName(const std::string &name);
        static bool isValidChar(const char &toTest);

    public:
        static std::string generate(const std::string &name, const std::vector<std::string> &usedNames = std::vector<std::string>());
    };
} // namespace mpc::file
