#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace mpc::file::mid::util
{

    class VariableLengthInt
    {

        int mValue = 0;
        std::vector<char> mBytes;
        int mSizeInBytes = 0;

    public:
        void setValue(int value);
        int getValue() const;
        int getByteCount() const;
        std::vector<char> getBytes();
        std::string toString() const;

    private:
        void parseBytes(std::stringstream &in);
        void buildBytes();

    public:
        VariableLengthInt();
        VariableLengthInt(int value);
        VariableLengthInt(std::stringstream &in);
    };
} // namespace mpc::file::mid::util
