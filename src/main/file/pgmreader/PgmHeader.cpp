#include "PgmHeader.hpp"

#include "MpcSpecs.h"

#include "file/pgmreader/ProgramFileReader.hpp"

#include "file/ByteUtil.hpp"
#include "Util.hpp"

#include <cassert>

using namespace mpc::file::pgmreader;

PgmHeader::PgmHeader(ProgramFileReader* programFile)
{
    auto pgmFileArray = programFile->readProgramFileArray();
	headerArray = Util::vecCopyOfRange(pgmFileArray, 0, 4);
}

std::vector<char> PgmHeader::getHeaderArray()
{
    return headerArray;
}

bool PgmHeader::verifyMagic()
{
	return headerArray[0] == PGM_HEADER_MAGIC[0] &&
           headerArray[1] == PGM_HEADER_MAGIC[1];
}

const uint16_t PgmHeader::getSoundCount()
{
    const auto result =
            ByteUtil::bytes2ushort({headerArray[2], headerArray[3]});

    assert(result <= MAX_SOUND_COUNT_IN_MEMORY);

    return result;
}
