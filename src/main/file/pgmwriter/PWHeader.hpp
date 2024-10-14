#pragma once

#include <vector>
#include <cstdint>

namespace mpc::file::pgmwriter {

    class PWHeader {

    private:
        const std::vector<char> PGM_HEADER_MAGIC{ 0x07, 0x04 };
        std::vector<char> headerArray;

    private:
        void writeMagic();

        void setSoundCount(uint16_t soundCount);

    public:
        std::vector<char> getHeaderArray();

    public:
        PWHeader(int numberOfSamples);
    };
}
