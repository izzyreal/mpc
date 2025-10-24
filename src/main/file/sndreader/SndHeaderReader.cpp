#include "SndHeaderReader.hpp"

#include "file/sndreader/SndReader.hpp"

#include <Util.hpp>
#include "file/ByteUtil.hpp"
#include <StrUtil.hpp>

using namespace mpc::file::sndreader;

SndHeaderReader::SndHeaderReader(SndReader* sndReader)
{
	auto sndFileArray = sndReader->getSndFileArray();
	headerArray = Util::vecCopyOfRange(sndFileArray, 0, 42);
}

std::vector<char>& SndHeaderReader::getHeaderArray()
{
    return headerArray;
}

bool SndHeaderReader::hasValidId()
{
    int i = headerArray[0];
    int j = headerArray[1];

	return (i == 1 && j < 5);
}

std::string SndHeaderReader::getName()
{
	std::string name;
	
	for (int i = 2; i < 18; i++) {
		if (headerArray[i] == 0x00) {
			break;
		}
		name.push_back(headerArray[i]);
	}


	return StrUtil::trim(name);
}

int SndHeaderReader::getLevel()
{
    return headerArray[19];
}

int SndHeaderReader::getTune()
{
    return headerArray[20];
}

bool SndHeaderReader::isMono()
{
    auto channels = headerArray[21];
    return channels == 0;
}

int SndHeaderReader::getStart()
{
    auto startArray = Util::vecCopyOfRange(headerArray, 22, 26);
	auto start = ByteUtil::bytes2uint(startArray);
    return start;
}

int SndHeaderReader::getEnd()
{
	auto endArray = Util::vecCopyOfRange(headerArray, 26, 30);
	auto end = ByteUtil::bytes2uint(endArray);
	return end;
}

int SndHeaderReader::getNumberOfFrames()
{
	auto numberOfFramesArray = Util::vecCopyOfRange(headerArray, 30, 34);
	int numberOfFrames = ByteUtil::bytes2uint(numberOfFramesArray);
	return numberOfFrames;
}

int SndHeaderReader::getLoopLength()
{
	auto loopLengthArray = Util::vecCopyOfRange(headerArray, 34, 38);
	auto loopLength = ByteUtil::bytes2uint(loopLengthArray);
	return loopLength;
}

bool SndHeaderReader::isLoopEnabled()
{
    int loop = headerArray[38];
    return loop == 1;
}

int SndHeaderReader::getNumberOfBeats()
{
    return headerArray[39];
}

int SndHeaderReader::getSampleRate()
{
	auto rateArray = Util::vecCopyOfRange(headerArray, 40, 42);
	auto rate = ByteUtil::bytes2ushort(rateArray);
	return static_cast<int>(rate);
}
