#include "Sequence.hpp"

#include "Mpc.hpp"
#include "SequenceStateManager.hpp"

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
    : getScreens(getScreens), getCurrentBarIndex(getCurrentBarIndex)
{
    stateManager = std::make_shared<SequenceStateManager>(this);

    for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
    {
        tracks.emplace_back(std::make_shared<Track>(
            trackIndex, this, getDefaultTrackName, getTickPosition, getScreens,
            isRecordingModeMulti, getActiveSequence, getAutoPunchMode, getBus,
            isEraseButtonPressed, isProgramPadPressed, sampler, eventHandler,
            isSixteenLevelsEnabled, getActiveTrackIndex, isRecording,
            isOverdubbing, isPunchEnabled, getPunchInTime, getPunchOutTime,
            isSoloEnabled));
    }

    tempoChangeTrack = std::make_shared<Track>(
        64, this, getDefaultTrackName, getTickPosition, getScreens,
        isRecordingModeMulti, getActiveSequence, getAutoPunchMode, getBus,
        isEraseButtonPressed, isProgramPadPressed, sampler, eventHandler,
        isSixteenLevelsEnabled, getActiveTrackIndex, isRecording, isOverdubbing,
        isPunchEnabled, getPunchInTime, getPunchOutTime, isSoloEnabled);
    tempoChangeTrack->setUsed(true);

    tracks.push_back(tempoChangeTrack);

    auto userScreen = getScreens()->get<ScreenId::UserScreen>();

    for (int i = 0; i < 33; i++)
    {
        deviceNames[i] = userScreen->getDeviceName(i);
    }
}

void Sequence::setLoopStart(const int l)
{
    loopStart = l;
}

int Sequence::getLoopStart() const
{
    return loopStart;
}

void Sequence::setLoopEnd(const int l)
{
    loopEnd = l;
}

int Sequence::getLoopEnd() const
{
    return loopEnd;
}

void Sequence::setFirstLoopBarIndex(const int i)
{
    firstLoopBarIndex = std::clamp(i, 0, lastLoopBarIndex);

    if (i > lastLoopBarIndex)
    {
        lastLoopBarIndex = i;
    }
}

int Sequence::getFirstLoopBarIndex() const
{
    return firstLoopBarIndex;
}

void Sequence::setLastLoopBarIndex(int i)
{
    if (i < 0)
    {
        i = 0;
    }

    const auto currentLastBarIndex = lastBarIndex.load();

    if (lastLoopBarEnd)
    {
        if (i < currentLastBarIndex)
        {
            lastLoopBarEnd = false;
            lastLoopBarIndex = currentLastBarIndex;
            return;
        }
        return;
    }
    if (i > currentLastBarIndex)
    {
        lastLoopBarEnd = true;
    }
    else
    {
        if (i < firstLoopBarIndex)
        {
            firstLoopBarIndex = i;
        }
    }

    lastLoopBarIndex = i;
}

int Sequence::getLastLoopBarIndex() const
{
    if (lastLoopBarEnd)
    {
        return lastBarIndex.load();
    }

    return lastLoopBarIndex;
}

bool Sequence::isLoopEnabled() const
{
    return loopEnabled;
}

void Sequence::setName(const std::string &s)
{
    name = s;
}

std::string Sequence::getName()
{
    if (!used)
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

void Sequence::setLastBarIndex(const int i)
{
    lastBarIndex.store(std::clamp(i, 0, 998));
}

int Sequence::getLastBarIndex() const
{
    return lastBarIndex.load();
}

int Sequence::getBarCount() const
{
    return lastBarIndex.load() + 1;
}

void Sequence::setLoopEnabled(const bool b)
{
    loopEnabled = b;
}

std::shared_ptr<Track> Sequence::getTrack(const int i)
{
    return tracks[i];
}

void Sequence::setUsed(const bool b)
{
    used = b;
}

bool Sequence::isUsed() const
{
    return used;
}

void Sequence::init(const int newLastBarIndex)
{
    const auto userScreen = getScreens()->get<ScreenId::UserScreen>();
    initialTempo = userScreen->tempo;
    loopEnabled = userScreen->loop;

    for (const auto &t : tracks)
    {
        t->setDeviceIndex(userScreen->device);
        t->setProgramChange(userScreen->pgm);
        t->setBusType(userScreen->busType);
        t->setVelocityRatio(userScreen->velo);
    }

    setLastBarIndex(newLastBarIndex);

    tempoTrackIsInitialized.store(false);
    tempoChangeTrack->removeEvents();
    addTempoChangeEvent(0, 1000);
    tempoTrackIsInitialized.store(true);

    initLoop();

    setTimeSignature(0, getLastBarIndex(), userScreen->timeSig.numerator,
                     userScreen->timeSig.denominator);
    used = true;
}

void Sequence::setBarLengths(
    const std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> &barLengths) const
{
    stateManager->enqueue(UpdateBarLengths{barLengths});
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
    const auto snapshot = stateManager->getSnapshot();

    const auto oldBarLength = snapshot.getBarLength(barIndex);
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

    stateManager->enqueue(UpdateBarLength{barIndex, newBarLength});
    getNumerators()[barIndex] = num;
    getDenominators()[barIndex] = den;
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
    if (!tempoTrackIsInitialized.load())
    {
        return {};
    }

    std::vector<std::shared_ptr<TempoChangeEvent>> res;

    for (auto t : tempoChangeTrack->getEvents())
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
    tempoChangeTrack->insertEvent(e);
}

double Sequence::getInitialTempo() const
{
    return initialTempo;
}

void Sequence::setInitialTempo(const double newInitialTempo)
{
    this->initialTempo = newInitialTempo;

    if (newInitialTempo < 30.0)
    {
        this->initialTempo = 30.0;
    }
    else if (newInitialTempo > 300.0)
    {
        this->initialTempo = 300.0;
    }
}

void Sequence::removeTempoChangeEvent(const int i) const
{
    tempoChangeTrack->removeEvent(i);
}

bool Sequence::isTempoChangeOn() const
{
    return tempoChangeOn;
}

void Sequence::setTempoChangeOn(const bool b)
{
    tempoChangeOn = b;
}

int Sequence::getLastTick() const
{
    int lastTick = 0;

    const auto snapshot = stateManager->getSnapshot();

    for (int i = 0; i < getLastBarIndex() + 1; i++)
    {
        lastTick += snapshot.getBarLength(i);
    }

    return lastTick;
}

TimeSignature Sequence::getTimeSignature() const
{
    auto ts = TimeSignature();
    int bar = getCurrentBarIndex();

    if (bar > lastBarIndex.load() && bar != 0)
    {
        bar--;
    }

    ts.numerator = TimeSigNumerator(numerators[bar]);
    ts.denominator = TimeSigDenominator(denominators[bar]);

    return ts;
}

void Sequence::purgeAllTracks()
{
    for (int i = 0; i < 64; i++)
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
    return denominators[i];
}

int Sequence::getNumerator(const int i) const
{
    return numerators[i];
}

void Sequence::deleteBars(const int firstBar, int lastBarToDelete)
{
    if (lastBarIndex.load() == -1)
    {
        return;
    }

    lastBarToDelete++;

    int deleteFirstTick = 0;

    const auto snapshot = stateManager->getSnapshot();

    for (int i = 0; i < firstBar; i++)
    {
        deleteFirstTick += snapshot.getBarLength(i);
    }

    int deleteLastTick = deleteFirstTick;

    for (int i = firstBar; i < lastBarToDelete; i++)
    {
        deleteLastTick += snapshot.getBarLength(i);
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

    const auto difference = lastBarToDelete - firstBar;
    int oldBarStartPos = 0;
    auto barCounter = 0;

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; ++i)
    {
        if (i == lastBarToDelete)
        {
            break;
        }

        oldBarStartPos += snapshot.getBarLength(i);
        barCounter++;
    }

    int newBarStartPos = 0;
    barCounter = 0;

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; ++i)
    {
        if (barCounter == firstBar)
        {
            break;
        }

        newBarStartPos += snapshot.getBarLength(i);
        barCounter++;
    }

    const auto tickDifference = oldBarStartPos - newBarStartPos;

    for (int i = firstBar; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        if (i + difference > Mpc2000XlSpecs::MAX_LAST_BAR_INDEX)
        {
            break;
        }

        stateManager->enqueue(
            UpdateBarLength{i, snapshot.getBarLength(i + difference)});
        numerators[i] = numerators[i + difference];
        denominators[i] = denominators[i + difference];
    }

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

    const int newLastBarIndex = lastBarIndex.load() - difference;

    lastBarIndex.store(newLastBarIndex);

    if (firstLoopBarIndex > newLastBarIndex)
    {
        firstLoopBarIndex = newLastBarIndex;
    }

    if (lastLoopBarIndex > newLastBarIndex)
    {
        lastLoopBarIndex = newLastBarIndex;
    }

    if (newLastBarIndex == -1)
    {
        setUsed(false);
    }
}

void Sequence::insertBars(int barCount, const int afterBar)
{
    const auto oldLastBarIndex = lastBarIndex.load();
    const bool isAppending = afterBar - 1 == oldLastBarIndex;

    if (oldLastBarIndex + barCount > Mpc2000XlSpecs::MAX_LAST_BAR_INDEX)
    {
        barCount = Mpc2000XlSpecs::MAX_LAST_BAR_INDEX - oldLastBarIndex;
    }

    if (barCount == 0)
    {
        return;
    }

    const auto newLastBarIndex = oldLastBarIndex + barCount;

    const auto snapshot = stateManager->getSnapshot();

    lastBarIndex.store(newLastBarIndex);

    oldNumerators = numerators;
    oldDenominators = denominators;

    for (int i = Mpc2000XlSpecs::MAX_LAST_BAR_INDEX; i >= afterBar; i--)
    {
        if (i - barCount < 0)
        {
            break;
        }

        stateManager->enqueue(
            UpdateBarLength{i, snapshot.getBarLength(i - barCount)});
        numerators[i] = oldNumerators[i - barCount];
        denominators[i] = oldDenominators[i - barCount];
    }

    // The below are sane defaults for the fresh bars, but the
    // caller of this method is expected to set them to what they
    // should be, given the use case.
    for (int i = afterBar; i < afterBar + barCount; i++)
    {
        stateManager->enqueue(UpdateBarLength{i, 384});
        numerators[i] = 4;
        denominators[i] = 4;
    }

    int barStart = 0;
    auto barCounter = 0;

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; ++i)
    {
        if (barCounter == afterBar)
        {
            break;
        }

        barStart += snapshot.getBarLength(i);
        barCounter++;
    }

    barCounter = 0;

    if (!isAppending)
    {
        int newBarStart = 0;

        for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; ++i)
        {
            if (barCounter == afterBar + barCount)
            {
                break;
            }

            newBarStart += snapshot.getBarLength(i);
            barCounter++;
        }

        for (const auto &t : tracks)
        {
            if (t->getIndex() == TempoChangeTrackIndex)
            {
                continue;
            }

            for (const auto &e : t->getEvents())
            {
                if (e->getTick() >= barStart)
                {
                    e->setTick(e->getTick() + (newBarStart - barStart));
                }
            }
        }
    }

    if (newLastBarIndex != -1 && !isUsed())
    {
        setUsed(true);
    }
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

bool Sequence::isLastLoopBarEnd() const
{
    return lastLoopBarEnd;
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

void Sequence::initLoop()
{
    const auto currentLastBarIndex = lastBarIndex.load();

    if (firstLoopBarIndex == -1 && currentLastBarIndex >= 0)
    {
        firstLoopBarIndex = 0;
    }

    if (lastLoopBarIndex == -1 && currentLastBarIndex >= 0)
    {
        lastLoopBarIndex = currentLastBarIndex;
    }

    const auto firstBar = getFirstLoopBarIndex();
    const auto lastBar = getLastLoopBarIndex() + 1;
    int newLoopStart = 0;
    int newLoopEnd = 0;

    const auto snapshot = stateManager->getSnapshot();

    for (int i = 0; i < lastBar; i++)
    {
        if (i < firstBar)
        {
            newLoopStart += snapshot.getBarLength(i);
        }

        newLoopEnd += snapshot.getBarLength(i);
    }

    setLoopStart(newLoopStart);
    setLoopEnd(newLoopEnd);
}

std::vector<int> &Sequence::getNumerators()
{
    return numerators;
}

std::vector<int> &Sequence::getDenominators()
{
    return denominators;
}

void Sequence::setNumeratorsAndDenominators(
    const std::vector<int> &newNumerators,
    const std::vector<int> &newDenominators)
{
    numerators = newNumerators;
    denominators = newDenominators;
}

int Sequence::getFirstTickOfBeat(const int bar, const int beat) const
{
    const auto barStart = getFirstTickOfBar(bar);
    const auto den = denominators[bar];
    const auto beatTicks = static_cast<int>(96 * (4.0 / den));
    return barStart + beat * beatTicks;
}

int Sequence::getFirstTickOfBar(const int index) const
{
    int res = 0;

    const auto snapshot = stateManager->getSnapshot();

    for (int i = 0; i < index; i++)
    {
        res += snapshot.getBarLength(i);
    }

    return res;
}

int Sequence::getLastTickOfBar(const int index) const
{
    const auto snapshot = stateManager->getSnapshot();
    return getFirstTickOfBar(index) + snapshot.getBarLength(index) - 1;
}

void Sequence::syncTrackEventIndices(const int tick) const
{
    if (!isUsed() || tick > getLastTick())
    {
        return;
    }

    for (auto &t : tracks)
    {
        if (t->isUsed())
        {
            t->syncEventIndex(tick, tick);
        }
    }
}

Sequence::StartTime &Sequence::getStartTime()
{
    return startTime;
}

std::shared_ptr<SequenceStateManager> Sequence::getStateManager()
{
    return stateManager;
}

std::shared_ptr<Track> Sequence::getTempoChangeTrack()
{
    return tempoChangeTrack;
}
