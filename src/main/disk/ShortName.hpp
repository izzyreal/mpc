#pragma once

#include <string>
#include <vector>

namespace mpc::disk {
    class ShortName
    {

    private:
        static std::vector<char> ILLEGAL_CHARS_;
        static const char ASCII_SPACE{32};
        static ShortName DOT_;
        static ShortName DOT_DOT_;
        std::vector<char> nameBytes{};

    private:
        static std::vector<char> toCharArray_(std::string name, std::string ext);
        void init();

    public:
        int8_t checkSum();
        static ShortName get(std::string name);
        static bool canConvert(std::string nameExt);
        static ShortName parse(std::vector<char> data);
        void write(std::vector<char> dest);
        std::string asSimpleString();

    private:
        static void checkValidName(std::string name);
        static void checkValidExt(std::string ext);
        static void checkString(std::string str, std::string strType, int minLength, int maxLength);

    public:
        int hashCode();
        static void checkValidChars(std::vector<char> chars);

    public:
        ShortName(std::string nameExt);
        ShortName(std::string name, std::string ext);

    private:
        static std::vector<char> ILLEGAL_CHARS();

    public:
        static ShortName DOT();
        static ShortName DOT_DOT();
    };

}
