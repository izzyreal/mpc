#include <file/sndreader/SndReader.hpp>

#include <disk/MpcFile.hpp>
#include <file/sndreader/SndHeaderReader.hpp>
#include <sampler/Sampler.hpp>

#include <Logger.hpp>

using namespace mpc::file::sndreader;
using namespace std;

SndReader::SndReader(mpc::disk::MpcFile* soundFile)
: SndReader(soundFile->getBytes())
{
}

SndReader::SndReader(const vector<char>& loadBytes)
{
    sndFileArray = loadBytes;
    sndHeaderReader = make_shared<SndHeaderReader>(this);
}

bool SndReader::isHeaderValid()
{
    return sndHeaderReader->hasValidId();
}

string SndReader::getName()
{
    return sndHeaderReader->getName();
}

bool SndReader::isMono()
{
    return sndHeaderReader->isMono();
}

int SndReader::getNumberOfFrames()
{
    return sndHeaderReader->getNumberOfFrames();
}

int SndReader::getSampleRate()
{
    return sndHeaderReader->getSampleRate();
}

int SndReader::getLevel()
{
    return sndHeaderReader->getLevel();
}

int SndReader::getStart()
{
    return sndHeaderReader->getStart();
}

int SndReader::getEnd()
{
    return sndHeaderReader->getEnd();
}

int SndReader::getLoopLength()
{
    return sndHeaderReader->getLoopLength();
}

bool SndReader::isLoopEnabled()
{
    return sndHeaderReader->isLoopEnabled();
}

int SndReader::getTune()
{
    return sndHeaderReader->getTune();
}

int SndReader::getNumberOfBeats()
{
    return sndHeaderReader->getNumberOfBeats();
}

void SndReader::readData(vector<float>& dest)
{
	int length = sndHeaderReader->getNumberOfFrames();

	bool mono = sndHeaderReader->isMono();

	if (!mono)
        length *= 2;

	dest.clear();
	dest.resize(length);

	vector<short> shorts = moduru::VecUtil::BytesToShorts(vector<char>(sndFileArray.begin() + 42, sndFileArray.end()));
	
    for (int i = 0; i < length; ++i)
    {
		short value = shorts[i];
		auto f = static_cast<float>(value / 32768.0);
	
        if (f < -1)
			f = -1.0f;
		
		if (f > 1)
			f = 1.0f;

		dest[i] = f;
	}
}

vector<char>& SndReader::getSndFileArray()
{
	return sndFileArray;
}
