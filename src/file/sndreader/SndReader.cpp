#include <file/sndreader/SndReader.hpp>

#include <disk/MpcFile.hpp>
#include <file/sndreader/SndHeaderReader.hpp>
#include <sampler/Sampler.hpp>

#include <Logger.hpp>

using namespace mpc::file::sndreader;
using namespace std;

SndReader::SndReader(mpc::disk::MpcFile* soundFile)
{
	sndFile = soundFile;
	sndFileArray = sndFile->getBytes();
	sndHeader = new SndHeaderReader(this);
}

string SndReader::getName()
{
    return sndHeader->getName();
}

bool SndReader::isMono()
{
    return sndHeader->isMono();
}

int SndReader::getNumberOfFrames()
{
    return sndHeader->getNumberOfFrames();
}

int SndReader::getSampleRate()
{
    return sndHeader->getSampleRate();
}

int SndReader::getLevel()
{
    return sndHeader->getLevel();
}

int SndReader::getStart()
{
    return sndHeader->getStart();
}

int SndReader::getEnd()
{
    return sndHeader->getEnd();
}

int SndReader::getLoopLength()
{
    return sndHeader->getLoopLength();
}

bool SndReader::isLoopEnabled()
{
    return sndHeader->isLoopEnabled();
}

int SndReader::getTune()
{
    return sndHeader->getTune();
}

int SndReader::getNumberOfBeats()
{
    return sndHeader->getNumberOfBeats();
}

void SndReader::getSampleData(vector<float>* dest)
{
	int length = sndHeader->getNumberOfFrames();

	bool mono = sndHeader->isMono();

	if (!mono) length *= 2;

	dest->clear();
	dest->resize(length);

	vector<short> shorts = moduru::VecUtil::BytesToShorts(vector<char>(sndFileArray.begin() + 42, sndFileArray.end()));
	for (int i = 0; i < length; ++i) {
		short value = shorts[i];
		auto f = static_cast<float>(value / 32768.0);
		if (f < -1) {
			f = -1.0f;
		}
		if (f > 1) {
			f = 1.0f;
		}
		dest->at(i) = f;
	}
}

vector<char> SndReader::getSndFileArray()
{
	return sndFileArray;
}
