#pragma once

#include <vector>
#include <cstdint>

namespace mpc::file::pgmwriter
{
    class PWHeader
    {
        const std::vector<char> PGM_HEADER_MAGIC{0x07, 0x04};
        std::vector<char> headerArray;

        void writeMagic();

        void setSoundCount(uint16_t soundCount);

    public:
        std::vector<char> getHeaderArray();

        PWHeader(int numberOfSamples);
    };
} // namespace mpc::file::pgmwriter
