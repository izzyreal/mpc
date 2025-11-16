#include "Bus.hpp"

#include "IntTypes.hpp"
#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include "performance/PerformanceManager.hpp"
#include "sampler/Sampler.hpp"

#include <algorithm>

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::engine;

Bus::Bus(const BusType busType) : busType(busType) {}

DrumBus::DrumBus(
    const DrumBusIndex drumIndexToUse,
    const std::shared_ptr<performance::PerformanceManager> &performanceManager,
    const std::function<std::shared_ptr<sampler::Sampler>()> &getSamplerFn)
    : Bus(BusType::DRUM1 + drumIndexToUse), drumIndex(drumIndexToUse),
      performanceManager(performanceManager), getSamplerFn(getSamplerFn)
{
    receivePgmChange = true;
    receiveMidiVolume = true;

    for (int i = 0; i < 64; i++)
    {
        stereoMixerChannels.emplace_back(std::make_shared<StereoMixer>());
        indivFxMixerChannels.emplace_back(std::make_shared<IndivFxMixer>());
    }
}

DrumBusIndex DrumBus::getIndex() const
{
    return drumIndex;
}

void DrumBus::setProgramIndex(const ProgramIndex programIndexToUse)
{
    programIndex = programIndexToUse;
    performanceManager->registerSetDrumProgram(
        drumIndex, programIndex, getSamplerFn()->getProgram(programIndex));
}

ProgramIndex DrumBus::getProgramIndex() const
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

performance::Program DrumBus::getPerformanceProgram() const
{
    return performanceManager->getSnapshot().getDrumProgram(drumIndex);
}
performance::StereoMixer
DrumBus::getPerformanceStereoMixer(const DrumNoteNumber drumNoteNumber) const
{
    return performanceManager->getSnapshot().getDrum(drumIndex).getStereoMixer(
        drumNoteNumber);
}
performance::IndivFxMixer
DrumBus::getPerformanceIndivFxMixer(const DrumNoteNumber drumNoteNumber) const
{
    return performanceManager->getSnapshot().getDrum(drumIndex).getIndivFxMixer(
        drumNoteNumber);
}

MidiBus::MidiBus() : Bus(BusType::MIDI) {}
