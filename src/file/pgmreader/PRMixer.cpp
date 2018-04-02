#include <file/pgmreader/PRMixer.hpp>

#include <file/pgmreader/ProgramFileReader.hpp>
#include <file/pgmreader/SoundNames.hpp>

#include <VecUtil.hpp>

using namespace moduru;
using namespace mpc::file::pgmreader;
using namespace std;

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

vector<char> Mixer::getMixerArray()
{
    auto pgmFileArray = programFile->readProgramFileArray();
    if (mixerArray.size() == 0) mixerArray = VecUtil::CopyOfRange(&pgmFileArray, getMixerStart(), getMixerEnd());
    return mixerArray;
}

int Mixer::getEffectsOutput(int pad)
{
    auto effectsOutput = getMixerArray()[(pad * 6) + 0];
    return effectsOutput;
}

int Mixer::getVolume(int pad)
{
    auto volume = getMixerArray()[(pad * 6) + 1];
    return volume;
}

int Mixer::getPan(int pad)
{
    auto pan = getMixerArray()[(pad * 6) + 2];
    return pan;
}

int Mixer::getVolumeIndividual(int pad)
{
    auto volumeIndividual = getMixerArray()[(pad * 6) + 3];
    return volumeIndividual;
}

int Mixer::getOutput(int pad)
{
    auto output = getMixerArray()[(pad * 6) + 4];
    return output;
}

int Mixer::getEffectsSendLevel(int pad)
{
    auto effectsSendLevel = getMixerArray()[(pad * 6) + 5];
    return effectsSendLevel;
}
