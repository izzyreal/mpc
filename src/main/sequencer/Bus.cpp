#include "Bus.hpp"

#include "IntTypes.hpp"
#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include "performance/PerformanceManager.hpp"

#include <algorithm>

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::engine;

Bus::Bus(const BusType busType) : busType(busType) {}

DrumBus::DrumBus(
    const DrumBusIndex drumIndexToUse,
    const std::shared_ptr<performance::PerformanceManager> &performanceManager)
    : Bus(BusType::DRUM1 + drumIndexToUse), drumIndex(drumIndexToUse),
      performanceManager(performanceManager)
{
    receivePgmChange = true;
    receiveMidiVolume = true;

    auto dispatch = [performanceManager](performance::PerformanceMessage &&m)
    {
        performanceManager->enqueue(std::move(m));
    };

    for (int i = 0; i < 64; i++)
    {
        const auto drumNoteNumber = DrumNoteNumber(i + MinDrumNoteNumber);
        auto getStereoMixerSnapshot =
            [performanceManager, this, drumNoteNumber]
        {
            return performanceManager->getSnapshot()
                .getDrum(drumIndex)
                .getStereoMixer(drumNoteNumber);
        };

        auto getIndivFxMixerSnapshot =
            [performanceManager, this, drumNoteNumber]
        {
            return performanceManager->getSnapshot()
                .getDrum(drumIndex)
                .getIndivFxMixer(drumNoteNumber);
        };

        stereoMixerChannels.emplace_back(
            std::make_shared<StereoMixer>(
                getStereoMixerSnapshot,
                [this]{return drumIndex;},
                []{ return NoProgramIndex; },
                [drumNoteNumber] { return drumNoteNumber; },
                dispatch
                ));
        indivFxMixerChannels.emplace_back(
            std::make_shared<IndivFxMixer>(
                getIndivFxMixerSnapshot,
                [this]{return drumIndex;},
                []{ return NoProgramIndex; },
                [drumNoteNumber] { return drumNoteNumber; },
                dispatch));
    }
}

DrumBusIndex DrumBus::getIndex() const
{
    return drumIndex;
}

void DrumBus::setProgramIndex(const ProgramIndex programIndexToUse)
{
    programIndex = programIndexToUse;
    performanceManager->registerUpdateDrumProgram(drumIndex, programIndex);
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
