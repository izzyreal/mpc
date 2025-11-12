#pragma once

#include <vector>

namespace mpc::file::pgmreader
{

    class ProgramFileReader;

    class PgmHeader
    {

        const std::vector<char> PGM_HEADER_MAGIC{0x07, 0x04};
        std::vector<char> headerArray;

    public:
        std::vector<char> getHeaderArray();

        bool verifyMagic() const;

        const uint16_t getSoundCount();

        PgmHeader(ProgramFileReader *programFile);
    };
} // namespace mpc::file::pgmreader
