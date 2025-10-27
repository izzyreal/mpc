#include "Drum.hpp"

#include "StereoMixer.hpp"
#include "IndivFxMixer.hpp"

#include "MpcSpecs.hpp"

using namespace mpc::engine;

Drum::Drum(const int drumIndexToUse) : drumIndex(drumIndexToUse)
{
    receivePgmChange = true;
    receiveMidiVolume = true;

    for (int i = 0; i < 64; i++)
    {
        stereoMixerChannels.emplace_back(std::make_shared<StereoMixer>());
        indivFxMixerChannels.emplace_back(std::make_shared<IndivFxMixer>());
    }
}

void Drum::setProgram(int programIndexToUse)
{
    programIndex =
        std::clamp(programIndexToUse, 0, Mpc2000XlSpecs::MAX_PROGRAM_COUNT - 1);
}

int Drum::getProgram() const
{
    return programIndex;
}

bool Drum::receivesPgmChange() const
{
    return receivePgmChange;
}

void Drum::setReceivePgmChange(bool b)
{
    receivePgmChange = b;
}

bool Drum::receivesMidiVolume() const
{
    return receiveMidiVolume;
}

void Drum::setReceiveMidiVolume(bool b)
{
    receiveMidiVolume = b;
}

int Drum::getLastReceivedMidiVolume() const
{
    return lastReceivedMidiVolume;
}

void Drum::setLastReceivedMidiVolume(int volume)
{
    lastReceivedMidiVolume = std::clamp(volume, 0, 127);
}

std::vector<std::shared_ptr<StereoMixer>> &Drum::getStereoMixerChannels()
{
    return stereoMixerChannels;
}

std::vector<std::shared_ptr<IndivFxMixer>> &Drum::getIndivFxMixerChannels()
{
    return indivFxMixerChannels;
}
