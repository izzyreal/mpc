#include "Bus.hpp"

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include "MpcSpecs.hpp"

#include <algorithm>

using namespace mpc::sequencer;
using namespace mpc::engine;

DrumBus::DrumBus(const int drumIndexToUse) : drumIndex(drumIndexToUse)
{
    receivePgmChange = true;
    receiveMidiVolume = true;

    for (int i = 0; i < 64; i++)
    {
        stereoMixerChannels.emplace_back(std::make_shared<StereoMixer>());
        indivFxMixerChannels.emplace_back(std::make_shared<IndivFxMixer>());
    }
}

int DrumBus::getIndex() const
{
    return drumIndex;
}

void DrumBus::setProgram(int programIndexToUse)
{
    programIndex =
        std::clamp(programIndexToUse, 0,
                   static_cast<int>(Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX));
}

int DrumBus::getProgram() const
{
    return programIndex;
}

bool DrumBus::receivesPgmChange() const
{
    return receivePgmChange;
}

void DrumBus::setReceivePgmChange(bool b)
{
    receivePgmChange = b;
}

bool DrumBus::receivesMidiVolume() const
{
    return receiveMidiVolume;
}

void DrumBus::setReceiveMidiVolume(bool b)
{
    receiveMidiVolume = b;
}

int DrumBus::getLastReceivedMidiVolume() const
{
    return lastReceivedMidiVolume;
}

void DrumBus::setLastReceivedMidiVolume(int volume)
{
    lastReceivedMidiVolume = std::clamp(volume, 0, 127);
}

std::map<int, int> &DrumBus::getSimultA()
{
    return simultA;
}

std::map<int, int> &DrumBus::getSimultB()
{
    return simultB;
}

std::vector<std::shared_ptr<StereoMixer>> &DrumBus::getStereoMixerChannels()
{
    return stereoMixerChannels;
}

std::vector<std::shared_ptr<IndivFxMixer>> &DrumBus::getIndivFxMixerChannels()
{
    return indivFxMixerChannels;
}
