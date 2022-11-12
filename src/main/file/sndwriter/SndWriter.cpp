#include <file/sndwriter/SndWriter.hpp>

#include <file/sndwriter/SndHeaderWriter.hpp>
#include <sampler/Sound.hpp>

#include <file/ByteUtil.hpp>

using namespace mpc::file::sndwriter;

SndWriter::SndWriter(mpc::sampler::Sound* sound) 
{
    this->sound = sound;
    sndHeaderWriter = std::make_shared<SndHeaderWriter>(this);
    setValues();
}
const int SndWriter::HEADER_SIZE;

void SndWriter::setValues()
{
	setName(sound->getName());
	setMono(sound->isMono());
	setFramesCount(sound->getFrameCount());
	setSampleRate(sound->getSampleRate());
	setLevel(sound->getSndLevel());
	setStart(sound->getStart());
	setEnd(sound->getEnd());
	setLoopLength(sound->getEnd() - sound->getLoopTo());
	setLoopEnabled(sound->isLoopEnabled());
	setTune(sound->getTune());
	setBeatCount(sound->getBeatCount());
	setSampleData(*sound->getSampleData(), sound->isMono());
}

void SndWriter::setName(std::string s)
{
    sndHeaderWriter->setName(s);
}

void SndWriter::setMono(bool b)
{
    sndHeaderWriter->setMono(b);
}

void SndWriter::setFramesCount(int i)
{
    sndHeaderWriter->setFrameCount(i);
}

void SndWriter::setSampleRate(int i)
{
    sndHeaderWriter->setSampleRate(i);
}

void SndWriter::setLevel(int i)
{
    sndHeaderWriter->setLevel(i);
}

void SndWriter::setStart(int i)
{
    sndHeaderWriter->setStart(i);
}

void SndWriter::setEnd(int i)
{
    sndHeaderWriter->setEnd(i);
}

void SndWriter::setLoopLength(int i)
{
    sndHeaderWriter->setLoopLength(i);
}

void SndWriter::setLoopEnabled(bool b)
{
    sndHeaderWriter->setLoopEnabled(b);
}

void SndWriter::setTune(int i)
{
    sndHeaderWriter->setTune(i);
}

void SndWriter::setBeatCount(int i)
{
    sndHeaderWriter->setBeatCount(i);
}

void SndWriter::setSampleData(const std::vector<float>& fa, bool mono)
{
	sndFileArray = std::vector<char>(HEADER_SIZE + (fa.size() * 2));
	auto frames = mono ? fa.size() : fa.size() * 0.5;
	sndHeaderWriter->setFrameCount(frames);
	
	std::vector<char> buffer(2);
	auto sPos = 0;
	
	auto bytePos = SndWriter::HEADER_SIZE;
	for (int i = 0; i < fa.size(); i++) {
		buffer = moduru::file::ByteUtil::short2bytes(fa[sPos++] * 32768);
		sndFileArray[bytePos++] = buffer[0];
		sndFileArray[bytePos++] = buffer[1];
	}
}

std::vector<char> SndWriter::getSndFileArray()
{
	auto header = sndHeaderWriter->getHeaderArray();
	for (int i = 0; i < header.size(); i++) {
		sndFileArray[i] = header[i];
	}

	return sndFileArray;
}
