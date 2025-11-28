#include "sequencer/Track.hpp"

#include "EventStateToEventMapper.hpp"
#include "TrackStateView.hpp"
#include "sampler/Sampler.hpp"

#include "sequencer/Bus.hpp"
#include "sequencer/Event.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "audiomidi/EventHandler.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include <concurrentqueue.h>

#include <memory>

using namespace mpc::sequencer;
using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

constexpr int TickUnassignedWhileRecording = -2;

Track::Track(
    const std::function<std::shared_ptr<TrackStateView>()> &getSnapshot,
    const std::function<void(SequencerMessage&&)> &dispatch,
    const int trackIndex, Sequence *parent,
    const std::function<std::string(int)> &getDefaultTrackName,
    const std::function<int64_t()> &getTickPosition,
    const std::function<std::shared_ptr<Screens>()> &getScreens,
    const std::function<bool()> &isRecordingModeMulti,
    const std::function<std::shared_ptr<Sequence>()> &getActiveSequence,
    const std::function<int()> &getAutoPunchMode,
    const std::function<std::shared_ptr<Bus>(BusType)> &getSequencerBus,
    const std::function<bool()> &isEraseButtonPressed,
    const std::function<bool(ProgramPadIndex, ProgramIndex)>
        &isProgramPadPressed,
    const std::shared_ptr<Sampler> &sampler,
    const std::shared_ptr<audiomidi::EventHandler> &eventHandler,
    const std::function<bool()> &isSixteenLevelsEnabled,
    const std::function<int()> &getActiveTrackIndex,
    const std::function<bool()> &isRecording,
    const std::function<bool()> &isOverdubbing,
    const std::function<bool()> &isPunchEnabled,
    const std::function<int64_t()> &getPunchInTime,
    const std::function<int64_t()> &getPunchOutTime,
    const std::function<bool()> &isSoloEnabled)
    : getSnapshot(getSnapshot), dispatch(dispatch), trackIndex(trackIndex), parent(parent),
      getDefaultTrackName(getDefaultTrackName),
      getTickPosition(getTickPosition), getScreens(getScreens),
      isRecordingModeMulti(isRecordingModeMulti),
      getActiveSequence(getActiveSequence), getAutoPunchMode(getAutoPunchMode),
      getSequencerBus(getSequencerBus),
      isEraseButtonPressed(isEraseButtonPressed),
      isProgramPadPressed(isProgramPadPressed), sampler(sampler),
      eventHandler(eventHandler),
      isSixteenLevelsEnabled(isSixteenLevelsEnabled),
      getActiveTrackIndex(getActiveTrackIndex), isRecording(isRecording),
      isOverdubbing(isOverdubbing), isPunchEnabled(isPunchEnabled),
      getPunchInTime(getPunchInTime), getPunchOutTime(getPunchOutTime),
      isSoloEnabled(isSoloEnabled)
{
    init();
    bulkNoteOns.resize(20);
    bulkNoteOffs.resize(20);
}

Track::~Track()
{
    //    printf("~Track\n");
}

void Track::init()
{
    name = trackIndex == TempoChangeTrackIndex
           ? "tempo"
           : getDefaultTrackName(trackIndex);
    programChange = 0;
    velocityRatio = 100;
    used = false;
    on = true;
    device = 0;
    busType = BusType::DRUM1;
    queuedNoteOnEvents =
        std::make_shared<moodycamel::ConcurrentQueue<EventState>>(20);
    queuedNoteOffEvents =
        std::make_shared<moodycamel::ConcurrentQueue<EventState>>(20);
}

void Track::purge()
{
    init();
    bulkNoteOns.clear();
    bulkNoteOns.resize(20);
    bulkNoteOffs.clear();
    bulkNoteOffs.resize(20);
    removeEvents();
}

EventState Track::findRecordingNoteOnEventById(const NoteEventId id)
{
    EventState found;
    EventState e;

    size_t count = 0;

    bool foundInQueue = false;

    while (count < bulkNoteOns.size() && queuedNoteOnEvents->try_dequeue(e))
    {
        if (e.noteEventId == id)
        {
            found = e;
            foundInQueue = true;
        }

        assert(e.beingRecorded);
        bulkNoteOns[count++] = e;
    }

    for (size_t i = 0; i < count; i++)
    {
        queuedNoteOnEvents->enqueue(bulkNoteOns[i]);
    }

    if (!foundInQueue)
    {
        found =
            getSnapshot()->findRecordingNoteOnByNoteEventId(
                id);
    }

    return found;
}

EventState
Track::findRecordingNoteOnEventByNoteNumber(const NoteNumber noteNumber)
{
    EventState found;
    EventState e;

    size_t count = 0;

    bool foundInQueue = false;

    while (count < bulkNoteOns.size() && queuedNoteOnEvents->try_dequeue(e))
    {
        if (e.beingRecorded && e.noteNumber == noteNumber)
        {
            found = e;
            foundInQueue = true;
        }

        bulkNoteOns[count++] = e;
    }

    for (size_t i = 0; i < count; i++)
    {
        queuedNoteOnEvents->enqueue(bulkNoteOns[i]);
    }

    if (!foundInQueue)
    {
        found =
            getSnapshot()->findRecordingNoteOnByNoteNumber(
                noteNumber);
    }

    return found;
}

void Track::printEvents() const
{
    for (const auto &e : getEvents())
    {
        auto snapshot = e->getSnapshot();
        printf("Track event at tick %lld with note number %i\n", snapshot.tick,
               snapshot.noteNumber.get());
    }
}

void Track::setEventStates(const std::vector<EventState> &eventStates) const
{
    dispatch(UpdateEvents{parent->getSequenceIndex(), getIndex(), eventStates});
}

void Track::setTrackIndex(const TrackIndex i)
{
    trackIndex = i;
    dispatch(UpdateTrackIndexOfAllEvents{parent->getSequenceIndex(), i});
}

mpc::TrackIndex Track::getIndex() const
{
    return trackIndex;
}

void Track::flushNoteCache() const
{
    EventState e;
    while (queuedNoteOnEvents->try_dequeue(e))
    {
    }
    while (queuedNoteOffEvents->try_dequeue(e))
    {
    }
}

void Track::setUsed(const bool b)
{
    if (!used && b && trackIndex < 64)
    {
        name = getDefaultTrackName(trackIndex);
    }

    used = b;
}

void Track::setOn(const bool b)
{
    on = b;
}

void Track::removeEvent(const std::shared_ptr<Event> &event) const
{
    dispatch(RemoveEvent{parent->getSequenceIndex(), getIndex(), event->getSnapshot().eventId});
}

EventState Track::recordNoteEventLive(const NoteNumber note,
                                      const Velocity velocity)
{
    const NoteEventId noteEventIdToUse = nextNoteEventId;
    nextNoteEventId = getNextNoteEventId(nextNoteEventId);

    EventState e;
    e.type = EventType::NoteOn;
    e.noteNumber = note;
    e.velocity = velocity;
    e.noteEventId = noteEventIdToUse;
    e.beingRecorded = true;
    e.sequenceIndex = parent->getSequenceIndex();
    e.trackIndex = trackIndex;
    e.tick = TickUnassignedWhileRecording;
    queuedNoteOnEvents->enqueue(e);
    return e;
}

void Track::finalizeNoteEventLive(const EventState &noteOnEvent) const
{
    EventState e;
    e.type = EventType::NoteOff;
    e.noteNumber = noteOnEvent.noteNumber;
    e.tick = getTickPosition();
    queuedNoteOffEvents->enqueue(e);
}

EventState Track::recordNoteEventNonLive(const int tick, const NoteNumber note,
                                         const Velocity velocity,
                                         const int64_t metronomeOnlyTick)
{
    if (auto result =
            getSnapshot()->findNoteEvent(tick, note))
    {
        auto &noteEvent = *result;
        noteEvent.beingRecorded = true;
        noteEvent.velocity = velocity;
        noteEvent.duration = NoDuration;
        noteEvent.metronomeOnlyTickPosition = metronomeOnlyTick;
        dispatch(UpdateEvent{noteEvent});
        return noteEvent;
    }

    const auto noteEventIdToUse = nextNoteEventId;
    nextNoteEventId = getNextNoteEventId(nextNoteEventId);

    EventState noteEvent;
    noteEvent.type = EventType::NoteOn;
    noteEvent.noteEventId = noteEventIdToUse;
    noteEvent.noteNumber = note;
    noteEvent.velocity = velocity;
    noteEvent.sequenceIndex = parent->getSequenceIndex();
    noteEvent.trackIndex = trackIndex;
    noteEvent.tick = tick;
    noteEvent.beingRecorded = true;
    noteEvent.metronomeOnlyTickPosition = metronomeOnlyTick;
    insertEvent(noteEvent);
    return noteEvent;
}

void Track::finalizeNoteEventNonLive(const EventState &noteOnEvent,
                                     const Duration duration) const
{
    dispatch(FinalizeNonLiveNoteEvent{noteOnEvent, duration});
}

void Track::removeEvent(const EventId eventId) const
{
    dispatch(RemoveEvent{parent->getSequenceIndex(), getIndex(), eventId});
}

void Track::removeEvents() const
{
    dispatch(ClearEvents{parent->getSequenceIndex(), getIndex()});
}

void Track::setVelocityRatio(int i)
{
    if (i < 1)
    {
        i = 1;
    }
    else if (i > 200)
    {
        i = 200;
    }

    velocityRatio = i;
}

int Track::getVelocityRatio() const
{
    return velocityRatio;
}

void Track::setProgramChange(const int i)
{
    programChange = std::clamp(i, 0, 128);
}

int Track::getProgramChange() const
{
    return programChange;
}

void Track::setBusType(BusType bt)
{
    using U = std::underlying_type_t<BusType>;
    constexpr U MIN = static_cast<U>(BusType::MIDI);
    constexpr U MAX = static_cast<U>(BusType::DRUM4);

    const U raw = static_cast<U>(bt);
    const U clamped = std::clamp(raw, MIN, MAX);

    busType = static_cast<BusType>(clamped);
}

BusType Track::getBusType() const
{
    return busType;
}

void Track::setDeviceIndex(const int i)
{
    device = std::clamp(i, 0, 32);
}

int Track::getDeviceIndex() const
{
    return device;
}

std::shared_ptr<Event> Track::getEvent(const int i) const
{
    const auto eventState =
        getSnapshot()->getEventByIndex(EventIndex(i));
    return mapEventStateToEvent(getSnapshot, eventState, dispatch,
                                parent);
}

void Track::setName(const std::string &s)
{
    name = s;
}

std::string Track::getName()
{
    if (!used)
    {
        return "(Unused)";
    }
    return name;
}

std::vector<EventState> Track::getEventStates() const
{
    return getSnapshot()->getEvents();
}

mpc::EventId Track::getAndIncrementNextEventId()
{
    const EventId eventIdToUse = nextEventId;
    nextEventId = getNextEventId(nextEventId);
    return eventIdToUse;
}

std::vector<std::shared_ptr<Event>> Track::getEvents() const
{
    std::vector<std::shared_ptr<Event>> result;

    const auto snapshot = getSnapshot();

    const int eventCount = snapshot->getEventCount();

    for (int i = 0; i < eventCount; ++i)
    {
        auto event = mapEventStateToEvent(
            getSnapshot, snapshot->getEventByIndex(EventIndex(i)),
            dispatch, parent);
        result.emplace_back(event);
    }

    return result;
}

int Track::getCorrectedTickPos() const
{
    const auto pos = getTickPosition();
    auto correctedTickPos = -1;

    const auto timingCorrectScreen =
        getScreens()->get<ScreenId::TimingCorrectScreen>();
    const auto swingPercentage = timingCorrectScreen->getSwing();
    const auto noteValueLengthInTicks =
        timingCorrectScreen->getNoteValueLengthInTicks();

    if (noteValueLengthInTicks > 1)
    {
        correctedTickPos =
            timingCorrectTick(0, parent->getLastBarIndex(), pos,
                              noteValueLengthInTicks, swingPercentage);
    }

    if (timingCorrectScreen->getAmount() != 0)
    {
        auto shiftedTick = correctedTickPos != -1 ? correctedTickPos : pos;
        auto amount = timingCorrectScreen->getAmount();

        if (!timingCorrectScreen->isShiftTimingLater())
        {
            amount *= -1;
        }

        shiftedTick += amount;

        if (shiftedTick < 0)
        {
            shiftedTick = 0;
        }

        if (const auto lastTick = parent->getLastTick(); shiftedTick > lastTick)
        {
            shiftedTick = lastTick;
        }

        correctedTickPos = shiftedTick;
    }

    return correctedTickPos;
}

void Track::processRealtimeQueuedEvents()
{
    const auto noteOnCount =
        queuedNoteOnEvents->try_dequeue_bulk(bulkNoteOns.begin(), 20);
    const auto noteOffCount =
        queuedNoteOffEvents->try_dequeue_bulk(bulkNoteOffs.begin(), 20);

    if (noteOnCount == 0 && noteOffCount == 0)
    {
        return;
    }

    const auto pos = getTickPosition();
    const auto correctedTickPos = getCorrectedTickPos();

    for (int noteOffIndex = 0; noteOffIndex < noteOffCount; noteOffIndex++)
    {
        if (auto noteOff = bulkNoteOffs[noteOffIndex];
            noteOff.tick == TickUnassignedWhileRecording)
        {
            noteOff.tick = pos;
        }
    }

    for (int noteOnIndex = 0; noteOnIndex < noteOnCount; noteOnIndex++)
    {
        auto noteOn = bulkNoteOns[noteOnIndex];

        if (noteOn.tick == TickUnassignedWhileRecording)
        {
            noteOn.tick = pos;

            if (correctedTickPos != pos && correctedTickPos != -1)
            {
                noteOn.tick = correctedTickPos;
            }

            noteOn.wasMoved = noteOn.tick - pos;
        }

        bool needsToBeRequeued = true;

        for (int noteOffIndex = 0; noteOffIndex < noteOffCount; noteOffIndex++)
        {
            if (auto noteOff = bulkNoteOffs[noteOffIndex];
                noteOff.noteNumber == noteOn.noteNumber)
            {
                auto newTick = noteOff.tick + noteOn.wasMoved;
                if (newTick < 0)
                {
                    newTick += parent->getLastTick();
                }
                else if (newTick > parent->getLastTick())
                {
                    newTick -= parent->getLastTick();
                }

                noteOff.tick = newTick;

                auto duration = noteOff.tick - noteOn.tick;

                if (noteOff.tick < noteOn.tick)
                {
                    duration = parent->getLastTick() - noteOn.tick;
                }

                if (duration < 1)
                {
                    duration = 1;
                }

                noteOn.duration = Duration(duration);
                insertEvent(noteOn);

                needsToBeRequeued = false;
            }
            else
            {
                queuedNoteOffEvents->enqueue(noteOff);
            }
        }

        if (needsToBeRequeued)
        {
            queuedNoteOnEvents->enqueue(noteOn);
        }
    }
}

bool Track::isOn() const
{
    return on;
}

bool Track::isUsed() const
{
    return used;
}

std::vector<std::shared_ptr<Event>>
Track::getEventRange(const int startTick, const int endTick) const
{
    std::vector<std::shared_ptr<Event>> result;
    for (const auto &e :
         getSnapshot()->getEventRange(startTick, endTick))
    {
        result.emplace_back(
            mapEventStateToEvent(getSnapshot, e, dispatch, parent));
    }
    return result;
}

void Track::correctTimeRange(const int startPos, const int endPos,
                             const int stepLength, const int swingPercentage,
                             const int lowestNote, const int highestNote) const
{
    const auto seq = getActiveSequence();
    int accumBarLengths = 0;
    auto fromBar = 0;
    auto toBar = 0;

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        accumBarLengths += seq->getBarLength(i);

        if (accumBarLengths >= startPos)
        {
            fromBar = i;
            break;
        }
    }

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        accumBarLengths += seq->getBarLength(i);

        if (accumBarLengths > endPos)
        {
            toBar = i;
            break;
        }
    }

    for (auto &event : getSnapshot()->getNoteEvents())
    {
        if (event.tick >= endPos)
        {
            break;
        }

        if (event.tick >= startPos && event.tick < endPos &&
            event.noteNumber >= lowestNote && event.noteNumber <= highestNote)
        {
            timingCorrect(fromBar, toBar, event.eventId, event.tick, stepLength,
                          swingPercentage);
        }
    }

    removeDoubles();
}

void Track::timingCorrect(const int fromBar, const int toBar,
                          const EventId eventId, const Tick eventTick,
                          const int stepLength, const int swingPercentage) const
{
    updateEventTick(eventId, timingCorrectTick(fromBar, toBar, eventTick,
                                               stepLength, swingPercentage));
}

int Track::timingCorrectTick(const int fromBar, const int toBar, int tick,
                             const int stepLength,
                             const int swingPercentage) const
{
    int accumBarLengths = 0;
    int previousAccumBarLengths = 0;
    auto barNumber = 0;
    auto numberOfSteps = 0;
    const auto seq = getActiveSequence();
    int segmentStart = 0;
    int segmentEnd = 0;

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        if (i < fromBar)
        {
            segmentStart += seq->getBarLength(i);
        }

        if (i <= toBar)
        {
            segmentEnd += seq->getBarLength(i);
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        accumBarLengths += seq->getBarLength(i);

        if (tick < accumBarLengths && tick >= previousAccumBarLengths)
        {
            barNumber = i;
            break;
        }

        previousAccumBarLengths = accumBarLengths;
    }

    for (int i = 1; i < 1000; i++)
    {
        if (seq->getBarLength(barNumber) - i * stepLength < 0)
        {
            numberOfSteps = i - 1;
            break;
        }
    }

    int currentBarStart = 0;

    for (int i = 0; i < barNumber; i++)
    {
        currentBarStart += seq->getBarLength(i);
    }

    for (int i = 0; i <= numberOfSteps; i++)
    {
        const int stepStart = (i - 1) * stepLength + stepLength / 2;

        if (const int stepEnd = i * stepLength + stepLength / 2;
            tick - currentBarStart >= stepStart &&
            tick - currentBarStart <= stepEnd)
        {
            tick = i * stepLength + currentBarStart;

            if (tick >= segmentEnd)
            {
                tick = segmentStart;
            }

            break;
        }
    }

    if ((stepLength == 24 || stepLength == 48) &&
        (tick + stepLength) % (stepLength * 2) == 0 && swingPercentage > 50)
    {
        const int swingOffset =
            (swingPercentage - 50.f) / 25.f * (stepLength / 2.f);
        tick += swingOffset;
    }

    return tick;
}

void Track::removeDoubles() const
{
    dispatch(RemoveDoubles{parent->getSequenceIndex(), getIndex()});
}

void Track::updateEventTick(const EventId eventId, const int newTick) const
{
    dispatch(UpdateEventTick{getSnapshot()->getEventById(eventId), newTick});
}

std::vector<std::shared_ptr<NoteOnEvent>> Track::getNoteEvents() const
{
    std::vector<std::shared_ptr<NoteOnEvent>> result;

    for (auto &e : getSnapshot()->getNoteEvents())
    {
        result.emplace_back(std::dynamic_pointer_cast<NoteOnEvent>(
            mapEventStateToEvent(getSnapshot, e, dispatch, parent)));
    }

    return result;
}

void Track::shiftTiming(const EventId eventId, const Tick eventTick,
                        const bool later, const int amount,
                        const int lastTick) const
{
    int amountToUse = amount;

    if (!later)
    {
        amountToUse *= -1;
    }

    int newEventTick = eventTick + amountToUse;

    if (newEventTick < 0)
    {
        newEventTick = 0;
    }
    else if (newEventTick > lastTick)
    {
        newEventTick = lastTick;
    }

    updateEventTick(eventId, newEventTick);
}

std::string Track::getActualName()
{
    return name;
}

void Track::insertEvent(
    EventState &event, const bool allowMultipleNoteEventsWithSameNoteOnSameTick,
    const std::function<void()> &onComplete)
{
    if (!isUsed())
    {
        setUsed(true);
    }

    const EventId eventIdToUse = nextEventId;

    nextEventId = getNextEventId(nextEventId);

    event.sequenceIndex = parent->getSequenceIndex();
    event.trackIndex = trackIndex;
    event.eventId = eventIdToUse;

    dispatch(InsertEvent{
        event, allowMultipleNoteEventsWithSameNoteOnSameTick, onComplete});
}
