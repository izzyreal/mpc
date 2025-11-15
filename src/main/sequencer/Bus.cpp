#include "Bus.hpp"

#include "IntTypes.hpp"
#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include <algorithm>

using namespace mpc::sequencer;
using namespace mpc::engine;

Bus::Bus(const BusType busType) : busType(busType) {}

DrumBus::DrumBus(const DrumBusIndex drumIndexToUse,
                 std::function<void(const DrumBusIndex, const ProgramIndex)>
                     setProgramInPerformanceState)
    : Bus(BusType::DRUM1 + drumIndexToUse), drumIndex(drumIndexToUse),
      setProgramInPerformanceState(setProgramInPerformanceState)
{
    receivePgmChange = true;
    receiveMidiVolume = true;

    for (int i = 0; i < 64; i++)
    {
        stereoMixerChannels.emplace_back(std::make_shared<StereoMixer>());
        indivFxMixerChannels.emplace_back(std::make_shared<IndivFxMixer>());
    }
}

mpc::DrumBusIndex DrumBus::getIndex() const
{
    return drumIndex;
}

void DrumBus::setProgramIndex(const ProgramIndex programIndexToUse)
{
    programIndex = programIndexToUse;
    setProgramInPerformanceState(drumIndex, programIndex);
}

mpc::ProgramIndex DrumBus::getProgramIndex() const
{
    return programIndex;
}

bool DrumBus::receivesPgmChange() const
{
    return receivePgmChange;
}

void DrumBus::setReceivePgmChange(const bool b)
{
    receivePgmChange = b;
}

bool DrumBus::receivesMidiVolume() const
{
    return receiveMidiVolume;
}

void DrumBus::setReceiveMidiVolume(const bool b)
{
    receiveMidiVolume = b;
}

int DrumBus::getLastReceivedMidiVolume() const
{
    return lastReceivedMidiVolume;
}

void DrumBus::setLastReceivedMidiVolume(const int volume)
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

MidiBus::MidiBus() : Bus(BusType::MIDI) {}
