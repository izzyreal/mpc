#include "PWHeader.hpp"

#include "MpcSpecs.h"
#include "file/ByteUtil.hpp"

#include <cassert>

using namespace mpc::file::pgmwriter;

PWHeader::PWHeader(int numberOfSamples) 
{
    headerArray = std::vector<char>(4);
    writeMagic();
    setSoundCount(numberOfSamples);
    headerArray[3] = 0;
}

void PWHeader::writeMagic()
{
	headerArray[0] = PGM_HEADER_MAGIC[0];
	headerArray[1] = PGM_HEADER_MAGIC[1];
}

void PWHeader::setSoundCount(const uint16_t soundCount)
{
    assert(soundCount <= MAX_SOUND_COUNT_IN_MEMORY);
    const auto soundCountBytes = mpc::file::ByteUtil::ushort2bytes(soundCount);
    headerArray[2] = soundCountBytes[0];
    headerArray[3] = soundCountBytes[1];
}

std::vector<char> PWHeader::getHeaderArray()
{
	return headerArray;
}
