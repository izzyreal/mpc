#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace mpc::midi::util
{

    class VariableLengthInt
    {

    private:
        int mValue = 0;
        std::vector<char> mBytes;
        int mSizeInBytes = 0;

    public:
        void setValue(int value);
        int getValue();
        int getByteCount();
        std::vector<char> getBytes();
        std::string toString();

    private:
        void parseBytes(std::stringstream &in);
        void buildBytes();

    public:
        VariableLengthInt();
        VariableLengthInt(int value);
        VariableLengthInt(std::stringstream &in);
    };
} // namespace mpc::midi::util
