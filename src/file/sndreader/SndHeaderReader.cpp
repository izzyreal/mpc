#include <file/sndreader/SndHeaderReader.hpp>

#include <file/sndreader/SndReader.hpp>

#include <VecUtil.hpp>
#include <file/ByteUtil.hpp>
#include <lang/StrUtil.hpp>

using namespace mpc::file::sndreader;
using namespace std;

SndHeaderReader::SndHeaderReader(SndReader* sndReader)
{
	vector<char> sndFileArray = sndReader->getSndFileArray();
	headerArray = moduru::VecUtil::CopyOfRange(&sndFileArray, 0, 42);
}

vector<char>* SndHeaderReader::getHeaderArray()
{
    return &headerArray;
}

bool SndHeaderReader::verifyFirstTwoBytes()
{
    auto verifyFirstTwoBytes = false;
    int i = headerArray[0];
    int j = headerArray[1];
    if(i == 1 && j < 5) {
        verifyFirstTwoBytes = true;
    }
    return verifyFirstTwoBytes;
}

string SndHeaderReader::getName()
{
	string name = "";
	for (int i = 2; i < 18; i++) {
		if (headerArray[i] == 0x00) {
			break;
		}
		name.push_back(headerArray[i]);
	}
	name = moduru::lang::StrUtil::trim(name);
	return name;
}

int SndHeaderReader::getLevel()
{
    return headerArray[19];
}

int SndHeaderReader::getTune()
{
	char tune = headerArray[20];
	char result = tune;
    return result;
}

bool SndHeaderReader::isMono()
{
    auto channels = headerArray[21] + 1;
    auto mono = false;
    if(channels == 1)
        mono = true;

    return mono;
}

int SndHeaderReader::getStart()
{
    auto startArray = moduru::VecUtil::CopyOfRange(&headerArray, 22, 26);
	auto start = moduru::file::ByteUtil::bytes2uint(startArray);
    return start;
}

int SndHeaderReader::getEnd()
{
	auto endArray = moduru::VecUtil::CopyOfRange(&headerArray, 26, 30);
	auto end = moduru::file::ByteUtil::bytes2uint(endArray);
	return end;
}

int SndHeaderReader::getNumberOfFrames()
{
	vector<char> numberOfFramesArray = moduru::VecUtil::CopyOfRange(&headerArray, 30, 34);
	int numberOfFrames = moduru::file::ByteUtil::bytes2uint(numberOfFramesArray);
	return numberOfFrames;
}

int SndHeaderReader::getLoopLength()
{
	auto loopLengthArray = moduru::VecUtil::CopyOfRange(&headerArray, 34, 38);
	auto loopLength = moduru::file::ByteUtil::bytes2uint(loopLengthArray);
	return loopLength;
}

bool SndHeaderReader::isLoopEnabled()
{
    int loop = headerArray[38];
    auto loopEnabled = false;
    if(loop == 1)
        loopEnabled = true;

    return loopEnabled;
}

int SndHeaderReader::getNumberOfBeats()
{
    return headerArray[39];
}

int SndHeaderReader::getSampleRate()
{
	auto rateArray = moduru::VecUtil::CopyOfRange(&headerArray, 40, 42);
	auto rate = moduru::file::ByteUtil::bytes2ushort(rateArray);
	return static_cast<int>(rate) + 65536;
}
