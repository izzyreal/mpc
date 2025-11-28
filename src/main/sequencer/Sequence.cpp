#include "Sequence.hpp"

#include "Mpc.hpp"
#include "SequenceStateView.hpp"
#include "SequencerStateManager.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/TimeSignature.hpp"

#include "lcdgui/screens/UserScreen.hpp"

using namespace mpc::sequencer;
using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

Sequence::Sequence(
    SequenceIndex sequenceIndex,
    const std::function<std::shared_ptr<SequenceStateView>()>
        &getSnapshot,
    const std::function<void(SequencerMessage &&)> &dispatch,
    std::function<std::string(int)> getDefaultTrackName,
    std::function<int64_t()> getTickPosition,
    std::function<std::shared_ptr<Screens>()> getScreens,
    std::function<bool()> isRecordingModeMulti,
    std::function<std::shared_ptr<Sequence>()> getActiveSequence,
    std::function<int()> getAutoPunchMode,
    std::function<std::shared_ptr<Bus>(BusType)> getBus,
    std::function<bool()> isEraseButtonPressed,
    std::function<bool(ProgramPadIndex, ProgramIndex)> isProgramPadPressed,
    std::shared_ptr<Sampler> sampler,
    std::shared_ptr<audiomidi::EventHandler> eventHandler,
    std::function<bool()> isSixteenLevelsEnabled,
    std::function<int()> getActiveTrackIndex, std::function<bool()> isRecording,
    std::function<bool()> isOverdubbing, std::function<bool()> isPunchEnabled,
    std::function<int64_t()> getPunchInTime,
    std::function<int64_t()> getPunchOutTime,
    std::function<bool()> isSoloEnabled,
    std::function<int()> getCurrentBarIndex)
    : sequenceIndex(sequenceIndex), getSnapshot(getSnapshot),
      dispatch(dispatch), getScreens(getScreens),
      getCurrentBarIndex(getCurrentBarIndex)
{
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
    {
        std::function getTrackSnapshot = [trackIndex, getSnapshot]
        {
            return getSnapshot()->getTrack(trackIndex);
        };
        tracks.emplace_back(std::make_shared<Track>(
            getTrackSnapshot, dispatch, trackIndex, this,
            getDefaultTrackName, getTickPosition, getScreens,
            isRecordingModeMulti, getActiveSequence, getAutoPunchMode, getBus,
            isEraseButtonPressed, isProgramPadPressed, sampler, eventHandler,
            isSixteenLevelsEnabled, getActiveTrackIndex, isRecording,
            isOverdubbing, isPunchEnabled, getPunchInTime, getPunchOutTime,
            isSoloEnabled));
    }

    std::function getTempoTrackSnapshot = [getSnapshot]
    {
        return getSnapshot()->getTrack(TempoChangeTrackIndex);
    };

    auto tempoChangeTrack = std::make_shared<Track>(
        getTempoTrackSnapshot, dispatch, TempoChangeTrackIndex, this,
        getDefaultTrackName, getTickPosition, getScreens, isRecordingModeMulti,
        getActiveSequence, getAutoPunchMode, getBus, isEraseButtonPressed,
        isProgramPadPressed, sampler, eventHandler, isSixteenLevelsEnabled,
        getActiveTrackIndex, isRecording, isOverdubbing, isPunchEnabled,
        getPunchInTime, getPunchOutTime, isSoloEnabled);
    tempoChangeTrack->setUsed(true);

    tracks.push_back(tempoChangeTrack);

    auto userScreen = getScreens()->get<ScreenId::UserScreen>();

    for (int i = 0; i < 33; i++)
    {
        deviceNames[i] = userScreen->getDeviceName(i);
    }
}

Sequence::~Sequence()
{
    //    printf("~Sequence\n");
}

void Sequence::setEventStates(const std::vector<EventState> &eventStates) const
{
    dispatch(UpdateSequenceEvents{getSequenceIndex(), eventStates});
}

mpc::SequenceIndex Sequence::getSequenceIndex() const
{
    return sequenceIndex;
}

mpc::Tick Sequence::getLoopStartTick() const
{
    return getSnapshot()->getLoopStartTick();
}

mpc::Tick Sequence::getLoopEndTick() const
{
    return getSnapshot()->getLoopEndTick();
}

void Sequence::setFirstLoopBarIndex(const BarIndex i) const
{
    dispatch(SetFirstLoopBarIndex{getSequenceIndex(), i});
}

mpc::BarIndex Sequence::getFirstLoopBarIndex() const
{
    return getSnapshot()->getFirstLoopBarIndex();
}

void Sequence::setLastLoopBarIndex(const BarIndex i) const
{
    if (i >= getBarCount())
    {
        dispatch(SetLastLoopBarIndex{getSequenceIndex(), EndOfSequence});
        return;
    }

    dispatch(SetLastLoopBarIndex{getSequenceIndex(), i});
}

mpc::BarIndex Sequence::getLastLoopBarIndex() const
{
    return getSnapshot()->getLastLoopBarIndex();
}

bool Sequence::isLoopEnabled() const
{
    return getSnapshot()->isLoopEnabled();
}

void Sequence::setName(const std::string &s)
{
    name = s;
}

std::string Sequence::getName()
{
    if (!isUsed())
    {
        return std::string("(Unused)");
    }

    return name;
}

void Sequence::setDeviceName(const int i, const std::string &s)
{
    deviceNames[i] = s;
}

std::string Sequence::getDeviceName(const int i)
{
    return deviceNames[i];
}

void Sequence::setLastBarIndex(const int i) const
{
    dispatch(SetLastBarIndex{getSequenceIndex(), BarIndex(i)});
}

int Sequence::getLastBarIndex() const
{
    return getSnapshot()->getLastBarIndex();
}

int Sequence::getBarCount() const
{
    return getLastBarIndex() + 1;
}

void Sequence::setLoopEnabled(const bool b) const
{
    dispatch(SetLoopEnabled{getSequenceIndex(), b});
}

std::shared_ptr<Track> Sequence::getTrack(const int i)
{
    return tracks[i];
}

void Sequence::setUsed(const bool b) const
{
    dispatch(SetUsed{getSequenceIndex(), b});
}

bool Sequence::isUsed() const
{
    return getSnapshot()->isUsed();
}

void Sequence::init(const int newLastBarIndex)
{
    const auto userScreen = getScreens()->get<ScreenId::UserScreen>();
    setInitialTempo(userScreen->tempo);
    setLoopEnabled(userScreen->loop);

    purgeAllTracks();

    for (const auto &t : tracks)
    {
        t->setDeviceIndex(userScreen->device);
        t->setProgramChange(userScreen->pgm);
        t->setBusType(userScreen->busType);
        t->setVelocityRatio(userScreen->velo);
    }

    setLastBarIndex(newLastBarIndex);
    addTempoChangeEvent(0, 1000);

    std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> timeSignatures{};
    std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> barLengths{};

    for (int i = 0; i < getBarCount(); ++i)
    {
        timeSignatures[i] = userScreen->timeSig;
        barLengths[i] = timeSignatures[i].getBarLength();
    }

    setTimeSignatures(timeSignatures);
    setBarLengths(barLengths);

    setFirstLoopBarIndex(BarIndex(0));
    setLastLoopBarIndex(EndOfSequence);

    setUsed(true);
}

void Sequence::setBarLengths(
    const std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> &barLengths) const
{
    dispatch(UpdateBarLengths{getSequenceIndex(), barLengths});
}

void Sequence::setTimeSignature(const int firstBar, const int tsLastBar,
                                const int num, const int den)
{
    for (int i = firstBar; i <= tsLastBar; i++)
    {
        setTimeSignature(i, num, den);
    }
}

void Sequence::setTimeSignature(const int barIndex, const int num,
                                const int den)
{
    const auto newDenTicks = 96 * (4.0 / den);

    const auto barStart = getFirstTickOfBar(barIndex);
    const auto snapshot = getSnapshot();

    const auto oldBarLength = snapshot->getBarLength(barIndex);
    const auto newBarLength = static_cast<int>(newDenTicks * num);
    const auto tickShift = newBarLength - oldBarLength;

    if (newBarLength < oldBarLength)
    {
        // The bar will be cropped, so we may have to remove some events
        // if they fall outside the new bar's region.
        for (int tick = barStart + newBarLength; tick < barStart + oldBarLength;
             tick++)
        {
            for (const auto &t : getTracks())
            {
                for (int eventIndex = t->getEvents().size() - 1;
                     eventIndex >= 0; eventIndex--)
                {
                    if (t->getEvent(eventIndex)->getTick() == tick)
                    {
                        t->removeEvent(eventIndex);
                    }
                }
            }
        }
    }

    if (barIndex < Mpc2000XlSpecs::MAX_LAST_BAR_INDEX)
    {
        // We're changing the timesignature of not the last bar, so
        // all bars after the bar we're changing are shifting. Here we
        // shift all relevant event ticks.
        const auto nextBarStartTick = getFirstTickOfBar(barIndex + 1);

        for (const auto &t : getTracks())
        {
            for (int eventIndex = t->getEvents().size() - 1; eventIndex >= 0;
                 eventIndex--)
            {
                if (const auto event = t->getEvent(eventIndex);
                    event->getTick() >= nextBarStartTick &&
                    event->getTick() < getLastTick())
                {
                    event->setTick(event->getTick() + tickShift);
                }
            }
        }
    }

    const auto ts =
        TimeSignature{TimeSigNumerator(num), TimeSigDenominator(den)};

    dispatch(UpdateTimeSignature{getSequenceIndex(), barIndex, ts});
}

void Sequence::setTimeSignatures(
    const std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT>
        &timeSignatures) const
{
    dispatch(UpdateTimeSignatures{getSequenceIndex(), timeSignatures});
}

std::vector<std::shared_ptr<Track>> Sequence::getTracks()
{
    return tracks;
}

std::vector<std::string> &Sequence::getDeviceNames()
{
    return deviceNames;
}

void Sequence::setDeviceNames(const std::vector<std::string> &sa)
{
    deviceNames = sa;
}

std::vector<std::shared_ptr<TempoChangeEvent>>
Sequence::getTempoChangeEvents() const
{
    std::vector<std::shared_ptr<TempoChangeEvent>> res;

    for (auto t : getTempoChangeTrack()->getEvents())
    {
        res.push_back(std::dynamic_pointer_cast<TempoChangeEvent>(t));
    }

    return res;
}

void Sequence::addTempoChangeEvent(const int tick, const int amount) const
{
    EventState e;
    e.type = EventType::TempoChange;
    e.tick = tick;
    e.amount = amount;
    e.trackIndex = TempoChangeTrackIndex;
    getTempoChangeTrack()->insertEvent(e);
}

double Sequence::getInitialTempo() const
{
    return getSnapshot()->getInitialTempo();
}

void Sequence::setInitialTempo(const double initialTempo) const
{
    dispatch(SetInitialTempo{getSequenceIndex(),
                                  std::clamp(initialTempo, 30.0, 300.0)});
}

void Sequence::removeTempoChangeEvent(const int i) const
{
    getTempoChangeTrack()->removeEvent(i);
}

bool Sequence::isTempoChangeOn() const
{
    return getSnapshot()->isTempoChangeEnabled();
}

void Sequence::setTempoChangeOn(const bool b) const
{
    dispatch(SetTempoChangeEnabled{getSequenceIndex(), b});
}

int Sequence::getLastTick() const
{
    return getSnapshot()->getLastTick();
}

TimeSignature Sequence::getTimeSignature(const int barIndex) const
{
    return getSnapshot()->getTimeSignature(barIndex);
}

TimeSignature Sequence::getTimeSignature() const
{
    int bar = getCurrentBarIndex();

    if (bar > getLastBarIndex() && bar != 0)
    {
        bar--;
    }

    return getSnapshot()->getTimeSignature(bar);
}

void Sequence::purgeAllTracks()
{
    for (int i = 0; i < Mpc2000XlSpecs::TOTAL_TRACK_COUNT; i++)
    {
        purgeTrack(i);
    }
}

std::shared_ptr<Track> Sequence::purgeTrack(const int i)
{
    tracks[i]->purge();
    return tracks[i];
}

int Sequence::getDenominator(const int i) const
{
    return getSnapshot()->getTimeSignature(i).denominator;
}

int Sequence::getNumerator(const int i) const
{
    return getSnapshot()->getTimeSignature(i).numerator;
}

void Sequence::deleteBars(const int firstBar, int lastBarToDelete) const
{
    if (getLastBarIndex() == NoBarIndex)
    {
        return;
    }

    lastBarToDelete++;

    const auto snapshot = getSnapshot();
    const auto &oldTs = snapshot->getTimeSignatures();

    int deleteFirstTick = 0;
    for (int i = 0; i < firstBar; i++)
    {
        deleteFirstTick += snapshot->getBarLength(i);
    }

    int deleteLastTick = deleteFirstTick;
    for (int i = firstBar; i < lastBarToDelete; i++)
    {
        deleteLastTick += snapshot->getBarLength(i);
    }

    for (const auto &t : tracks)
    {
        auto events = t->getEvents();
        for (auto &e : events)
        {
            if (e->getTick() >= deleteFirstTick &&
                e->getTick() < deleteLastTick)
            {
                t->removeEvent(e);
            }
        }
    }

    const int difference = lastBarToDelete - firstBar;

    int oldBarStartPos = 0;
    for (int i = 0; i < lastBarToDelete; ++i)
    {
        oldBarStartPos += snapshot->getBarLength(i);
    }

    int newBarStartPos = 0;
    for (int i = 0; i < firstBar; ++i)
    {
        newBarStartPos += snapshot->getBarLength(i);
    }

    const int tickDifference = oldBarStartPos - newBarStartPos;

    for (const auto &t : tracks)
    {
        if (t->getIndex() >= TempoChangeTrackIndex)
        {
            continue;
        }
        for (const auto &e : t->getEvents())
        {
            if (e->getTick() >= oldBarStartPos)
            {
                e->setTick(e->getTick() - tickDifference);
            }
        }
    }

    std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> newTs{};

    int out = 0;
    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; ++i)
    {
        if (i >= firstBar && i < lastBarToDelete)
        {
            continue;
        }
        if (out >= Mpc2000XlSpecs::MAX_BAR_COUNT)
        {
            break;
        }
        newTs[out++] = oldTs[i];
    }
    for (; out < Mpc2000XlSpecs::MAX_BAR_COUNT; ++out)
    {
        newTs[out] = TimeSignature{};
    }

    setTimeSignatures(newTs);

    const int newLastBarIndex = getLastBarIndex() - difference;
    dispatch(
        SetLastBarIndex{getSequenceIndex(), BarIndex(newLastBarIndex)});

    setFirstLoopBarIndex(BarIndex(0));
    setLastLoopBarIndex(EndOfSequence);

    if (newLastBarIndex == NoBarIndex)
    {
        setUsed(false);
    }
}

void Sequence::insertBars(const int barCount, const BarIndex afterBar) const
{
    auto onComplete = [this](const BarIndex newLastBarIndex)
    {
        if (newLastBarIndex != NoBarIndex && !isUsed())
        {
            setUsed(true);
        }
    };

    dispatch(
        InsertBars{getSequenceIndex(), barCount, afterBar, onComplete});
}

bool trackIndexComparator(const std::shared_ptr<Track> &t0,
                          const std::shared_ptr<Track> &t1)
{
    return t0->getIndex() < t1->getIndex();
}

void Sequence::moveTrack(const int source, const int destination)
{
    if (source == destination)
    {
        return;
    }

    if (source > destination)
    {
        tracks[source]->setTrackIndex(TrackIndex(destination));

        for (int i = destination; i < source; i++)
        {
            const auto t = tracks[i];
            t->setTrackIndex(t->getIndex() + 1);
        }
    }

    if (destination > source)
    {
        tracks[source]->setTrackIndex(TrackIndex(destination));

        for (int i = source + 1; i <= destination; i++)
        {
            const auto t = tracks[i];
            t->setTrackIndex(t->getIndex() - 1);
        }
    }

    sort(begin(tracks), end(tracks), trackIndexComparator);
}

int Sequence::getEventCount() const
{
    auto counter = 0;

    for (auto &t : tracks)
    {
        if (t->getIndex() > 63)
        {
            break;
        }

        counter += t->getEvents().size();
    }

    return counter;
}

int Sequence::getFirstTickOfBeat(const int barIndex, const int beat) const
{
    const auto barStart = getFirstTickOfBar(barIndex);
    const auto den = getSnapshot()->getTimeSignature(barIndex).denominator;
    const auto beatTicks = static_cast<int>(96 * (4.0 / den));
    return barStart + beat * beatTicks;
}

int Sequence::getFirstTickOfBar(const int barIndex) const
{
    return getSnapshot()->getFirstTickOfBar(BarIndex(barIndex));
}

int Sequence::getLastTickOfBar(const int barIndex) const
{
    const auto snapshot = getSnapshot();
    return getFirstTickOfBar(barIndex) + snapshot->getBarLength(barIndex) - 1;
}

Sequence::StartTime &Sequence::getStartTime()
{
    return startTime;
}

mpc::Tick Sequence::getBarLength(const int barIndex) const
{
    return getSnapshot()->getBarLength(barIndex);
}

std::array<mpc::Tick, mpc::Mpc2000XlSpecs::MAX_BAR_COUNT>
Sequence::getBarLengths() const
{
    return getSnapshot()->getBarLengths();
}

std::array<TimeSignature, mpc::Mpc2000XlSpecs::MAX_BAR_COUNT>
Sequence::getTimeSignatures() const
{
    return getSnapshot()->getTimeSignatures();
}

std::shared_ptr<Track> Sequence::getTempoChangeTrack() const
{
    return tracks[TempoChangeTrackIndex];
}
