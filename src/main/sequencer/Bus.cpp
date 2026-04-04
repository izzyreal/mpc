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
    const std::weak_ptr<performance::PerformanceManager> &performanceManager)
    : Bus(BusType::DRUM1 + drumIndexToUse), drumIndex(drumIndexToUse),
      performanceManager(performanceManager)
{
    auto dispatch = [performanceManager](performance::PerformanceMessage &&m)
    {
        performanceManager.lock()->enqueue(std::move(m));
    };

    for (int i = 0; i < 64; i++)
    {
        const auto drumNoteNumber = DrumNoteNumber(i + MinDrumNoteNumber);
        auto getStereoMixerSnapshot = [performanceManager, this, drumNoteNumber]
        {
            return performanceManager.lock()
                ->getSnapshot()
                .getDrum(drumIndex)
                .getStereoMixer(drumNoteNumber);
        };

        auto getIndivFxMixerSnapshot =
            [performanceManager, this, drumNoteNumber]
        {
            return performanceManager.lock()
                ->getSnapshot()
                .getDrum(drumIndex)
                .getIndivFxMixer(drumNoteNumber);
        };

        stereoMixerChannels.emplace_back(std::make_shared<StereoMixer>(
            getStereoMixerSnapshot,
            [this]
            {
                return drumIndex;
            },
            []
            {
                return NoProgramIndex;
            },
            [drumNoteNumber]
            {
                return drumNoteNumber;
            },
            dispatch));
        indivFxMixerChannels.emplace_back(std::make_shared<IndivFxMixer>(
            getIndivFxMixerSnapshot,
            [this]
            {
                return drumIndex;
            },
            []
            {
                return NoProgramIndex;
            },
            [drumNoteNumber]
            {
                return drumNoteNumber;
            },
            dispatch));
    }
}

DrumBusIndex DrumBus::getIndex() const
{
    return drumIndex;
}

void DrumBus::setProgramIndex(const ProgramIndex programIndexToUse) const
{
    performanceManager.lock()->registerUpdateDrumProgram(drumIndex,
                                                         programIndexToUse);
}

ProgramIndex DrumBus::getProgramIndex() const
{
    return performanceManager.lock()
        ->getSnapshot()
        .getDrum(drumIndex)
        .programIndex;
}

bool DrumBus::receivesPgmChange() const
{
    return performanceManager.lock()
        ->getSnapshot()
        .getDrum(drumIndex)
        .receivePgmChangeEnabled;
}

void DrumBus::setReceivePgmChange(const bool b)
{
    auto drum = performanceManager.lock()->getSnapshot().getDrum(drumIndex);
    drum.receivePgmChangeEnabled = b;
    performanceManager.lock()->enqueue(performance::UpdateDrumBulk{drum});
}

bool DrumBus::receivesMidiVolume() const
{
    return performanceManager.lock()
        ->getSnapshot()
        .getDrum(drumIndex)
        .receiveMidiVolumeEnabled;
}

void DrumBus::setReceiveMidiVolume(const bool b)
{
    auto drum = performanceManager.lock()->getSnapshot().getDrum(drumIndex);
    drum.receiveMidiVolumeEnabled = b;
    performanceManager.lock()->enqueue(performance::UpdateDrumBulk{drum});
}

int DrumBus::getLastReceivedMidiVolume() const
{
    return performanceManager.lock()
        ->getSnapshot()
        .getDrum(drumIndex)
        .lastReceivedMidiVolume;
}

void DrumBus::setLastReceivedMidiVolume(const int volume)
{
    auto drum = performanceManager.lock()->getSnapshot().getDrum(drumIndex);
    drum.lastReceivedMidiVolume = MidiValue(std::clamp(volume, 0, 127));
    performanceManager.lock()->enqueue(performance::UpdateDrumBulk{drum});
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
    return performanceManager.lock()->getSnapshot().getDrumProgram(drumIndex);
}
performance::StereoMixer
DrumBus::getPerformanceStereoMixer(const DrumNoteNumber drumNoteNumber) const
{
    return performanceManager.lock()
        ->getSnapshot()
        .getDrum(drumIndex)
        .getStereoMixer(drumNoteNumber);
}
performance::IndivFxMixer
DrumBus::getPerformanceIndivFxMixer(const DrumNoteNumber drumNoteNumber) const
{
    return performanceManager.lock()
        ->getSnapshot()
        .getDrum(drumIndex)
        .getIndivFxMixer(drumNoteNumber);
}

MidiBus::MidiBus() : Bus(BusType::MIDI) {}
