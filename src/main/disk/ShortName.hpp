#pragma once

#include <string>
#include <vector>

namespace mpc::disk
{
    class ShortName
    {

        static std::vector<char> ILLEGAL_CHARS_;
        static const char ASCII_SPACE{32};
        static ShortName DOT_;
        static ShortName DOT_DOT_;
        std::vector<char> nameBytes{};

        static std::vector<char> toCharArray_(const std::string &name,
                                              const std::string &ext);
        void init() const;

    public:
        int8_t checkSum() const;
        static ShortName get(const std::string &name);
        static bool canConvert(const std::string &nameExt);
        static ShortName parse(std::vector<char> data);
        void write(std::vector<char> dest) const;
        std::string asSimpleString() const;

    private:
        static void checkValidName(const std::string &name);
        static void checkValidExt(const std::string &ext);
        static void checkString(std::string str, std::string strType,
                                int minLength, int maxLength);

    public:
        int hashCode() const;
        static void checkValidChars(const std::vector<char> &chars);

        ShortName(std::string nameExt);
        ShortName(const std::string &name, const std::string &ext);

    private:
        static std::vector<char> ILLEGAL_CHARS();

    public:
        static ShortName DOT();
        static ShortName DOT_DOT();
    };

} // namespace mpc::disk
