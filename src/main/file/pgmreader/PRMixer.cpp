#include <file/pgmreader/PRMixer.hpp>

#include <file/pgmreader/ProgramFileReader.hpp>
#include <file/pgmreader/SoundNames.hpp>

#include <VecUtil.hpp>

using namespace moduru;
using namespace mpc::file::pgmreader;

Mixer::Mixer(ProgramFileReader* programFile) 
{
    this->programFile = programFile;
}

int Mixer::getSampleNamesSize()
{
	auto sampleNamesSize = programFile->getSampleNames()->getSampleNamesSize();
	return sampleNamesSize;
}

int Mixer::getMixerStart()
{
	auto mixerStart = 4 + getSampleNamesSize() + 2 + 17 + 9 + 6 + 1601;
	return mixerStart;
}

int Mixer::getMixerEnd()
{
	auto mixerEnd = 4 + getSampleNamesSize() + 2 + 17 + 9 + 6 + 1601 + 387;
	return mixerEnd;
}

std::vector<char> Mixer::getMixerArray()
{
    auto pgmFileArray = programFile->readProgramFileArray();
    if (mixerArray.size() == 0) mixerArray = VecUtil::CopyOfRange(pgmFileArray, getMixerStart(), getMixerEnd());
    return mixerArray;
}

int Mixer::getEffectsOutput(int noteIndex)
{
    auto effectsOutput = getMixerArray()[(noteIndex * 6) + 0];
    return effectsOutput;
}

int Mixer::getVolume(int noteIndex)
{
    auto volume = getMixerArray()[(noteIndex * 6) + 1];
    return volume;
}

int Mixer::getPan(int noteIndex)
{
    auto pan = getMixerArray()[(noteIndex * 6) + 2];
    return pan;
}

int Mixer::getVolumeIndividual(int noteIndex)
{
    auto volumeIndividual = getMixerArray()[(noteIndex * 6) + 3];
    return volumeIndividual;
}

int Mixer::getOutput(int noteIndex)
{
    auto output = getMixerArray()[(noteIndex * 6) + 4];
    return output;
}

int Mixer::getEffectsSendLevel(int noteIndex)
{
    auto effectsSendLevel = getMixerArray()[(noteIndex * 6) + 5];
    return effectsSendLevel;
}
