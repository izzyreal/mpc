#include "sequencer/Track.hpp"

#include "EventStateToEventMapper.hpp"
#include "SequencerStateManager.hpp"
#include "TrackStateView.hpp"
#include "sampler/Sampler.hpp"

#include "sequencer/Bus.hpp"
#include "sequencer/Event.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "audiomidi/EventHandler.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"

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
    const std::shared_ptr<SequencerStateManager> &manager,
    const std::function<std::shared_ptr<TrackStateView>()> &getSnapshot,
    const std::function<void(TrackMessage &&)> &dispatch, const int trackIndex,
    Sequence *parent,
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
    : manager(manager), getSnapshot(getSnapshot), dispatch(dispatch),
      trackIndex(trackIndex), parent(parent),
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

Track::~Track() {}

void Track::init()
{
    name = trackIndex == TempoChangeTrackIndex
               ? "tempo"
               : getDefaultTrackName(trackIndex);
    queuedNoteOnEvents =
        std::make_shared<moodycamel::ConcurrentQueue<EventState *>>(20);
    queuedNoteOffEvents =
        std::make_shared<moodycamel::ConcurrentQueue<EventState *>>(20);
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

EventState *Track::findRecordingNoteOnEvent(const EventState *eventState)
{
    EventState *found = nullptr;

    const auto count = queuedNoteOnEvents->try_dequeue_bulk(bulkNoteOns.begin(),
                                                            bulkNoteOns.size());

    for (int i = 0; i < count; i++)
    {
        const auto e = bulkNoteOns[i];
        if (e == eventState)
        {
            found = e;
            break;
        }
    }

    for (size_t i = 0; i < count; i++)
    {
        queuedNoteOnEvents->enqueue(bulkNoteOns[i]);
    }

    bulkNoteOns.clear();
    bulkNoteOns.resize(20);

    if (!found)
    {
        found = getSnapshot()->findRecordingNoteOn(eventState);
    }

    return found;
}

EventState *
Track::findRecordingNoteOnEventByNoteNumber(const NoteNumber noteNumber)
{
    EventState *found = nullptr;

    const auto count = queuedNoteOnEvents->try_dequeue_bulk(bulkNoteOns.begin(),
                                                            bulkNoteOns.size());

    for (int i = 0; i < count; i++)
    {
        const auto e = bulkNoteOns[i];
        if (e->noteNumber == noteNumber)
        {
            found = e;
            break;
        }
    }

    for (size_t i = 0; i < count; i++)
    {
        queuedNoteOnEvents->enqueue(bulkNoteOns[i]);
    }

    bulkNoteOns.clear();
    bulkNoteOns.resize(20);

    if (!found)
    {
        found = getSnapshot()->findRecordingNoteOnByNoteNumber(noteNumber);
    }

    return found;
}

void Track::printEvents() const
{
    for (const auto &e : getEvents())
    {
        const auto eventState = e->eventState;
        printf("Track event at tick %lld with note number %i\n",
               eventState->tick, eventState->noteNumber.get());
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
    EventState *e;
    while (queuedNoteOnEvents->try_dequeue(e))
    {
    }
    while (queuedNoteOffEvents->try_dequeue(e))
    {
    }
}

void Track::setUsed(const bool b)
{
    if (!isUsed() && b && trackIndex < 64)
    {
        name = getDefaultTrackName(trackIndex);
    }

    dispatch(SetTrackUsed{parent->getSequenceIndex(), getIndex(), b});
}

void Track::setOn(const bool b) const
{
    dispatch(SetTrackOn{parent->getSequenceIndex(), getIndex(), b});
}

void Track::syncEventIndex(const int currentTick, const int previousTick) const
{
    if (currentTick == 0)
    {
        dispatch(SetPlayEventIndex{parent->getSequenceIndex(), getIndex(),
                                   EventIndex(0)});
        return;
    }

    int startIndex = 0;

    const auto snapshot = getSnapshot();
    const auto eventCount = snapshot->getEventCount();
    const auto currentPlayEventIndex = snapshot->getPlayEventIndex();

    if (currentTick > previousTick)
    {
        if (currentPlayEventIndex == eventCount)
        {
            return;
        }

        startIndex = currentPlayEventIndex;
    }

    if (currentTick < previousTick && currentPlayEventIndex == 0)
    {
        return;
    }

    auto result{EventIndex(eventCount)};

    for (int i = startIndex; i < eventCount; i++)
    {
        if (snapshot->getEventByIndex(EventIndex(i))->tick >= currentTick)
        {
            result = EventIndex(i);
            break;
        }
    }

    dispatch(SetPlayEventIndex{parent->getSequenceIndex(), getIndex(), result});
}

void Track::removeEvent(const std::shared_ptr<Event> &event) const
{
    dispatch(
        RemoveEvent{parent->getSequenceIndex(), getIndex(), event->eventState});
}

EventState *Track::recordNoteEventLive(const NoteNumber note,
                                       const Velocity velocity) const
{
    EventState *e = manager->acquireEvent();
    e->type = EventType::NoteOn;
    e->noteNumber = note;
    e->velocity = velocity;
    e->beingRecorded = true;
    e->sequenceIndex = parent->getSequenceIndex();
    e->trackIndex = trackIndex;
    e->tick = TickUnassignedWhileRecording;
    queuedNoteOnEvents->enqueue(e);
    return e;
}

void Track::finalizeNoteEventLive(EventState *e) const
{
    e->tick = getTickPosition();
    queuedNoteOffEvents->enqueue(e);
}

EventState *Track::recordNoteEventNonLive(const int tick, const NoteNumber note,
                                          const Velocity velocity,
                                          const int64_t metronomeOnlyTick)
{
    if (const auto result = getSnapshot()->findNoteEvent(tick, note))
    {
        result->beingRecorded = true;
        result->velocity = velocity;
        result->duration = NoDuration;
        result->metronomeOnlyTickPosition = metronomeOnlyTick;
        return result;
    }

    EventState *result = manager->acquireEvent();
    result->type = EventType::NoteOn;
    result->noteNumber = note;
    result->velocity = velocity;
    result->sequenceIndex = parent->getSequenceIndex();
    result->trackIndex = trackIndex;
    result->tick = tick;
    result->beingRecorded = true;
    result->metronomeOnlyTickPosition = metronomeOnlyTick;
    insertEvent(result);
    return result;
}

void Track::finalizeNoteEventNonLive(EventState *noteOnEvent,
                                     const Duration duration) const
{
    dispatch(FinalizeNonLiveNoteEvent{noteOnEvent, duration});
}

void Track::removeEvent(EventState *eventState) const
{
    assert(eventState && eventState->trackIndex == getIndex() &&
           eventState->sequenceIndex == parent->getSequenceIndex());
    dispatch(RemoveEvent{parent->getSequenceIndex(), getIndex(), eventState});
}

void Track::removeEvents() const
{
    dispatch(ClearEvents{parent->getSequenceIndex(), getIndex()});
}

void Track::setVelocityRatio(int i) const
{
    if (i < 1)
    {
        i = 1;
    }
    else if (i > 200)
    {
        i = 200;
    }

    dispatch(SetTrackVelocityRatio{parent->getSequenceIndex(), getIndex(),
                                   static_cast<uint8_t>(i)});
}

int Track::getVelocityRatio() const
{
    return getSnapshot()->getVelocityRatio();
}

void Track::setProgramChange(const int i) const
{
    dispatch(
        SetTrackProgramChange{parent->getSequenceIndex(), getIndex(),
                              static_cast<uint8_t>(std::clamp(i, 0, 128))});
}

int Track::getProgramChange() const
{
    return getSnapshot()->getProgramChange();
}

void Track::setBusType(BusType bt) const
{
    using U = std::underlying_type_t<BusType>;
    constexpr U MIN = static_cast<U>(BusType::MIDI);
    constexpr U MAX = static_cast<U>(BusType::DRUM4);

    const U raw = static_cast<U>(bt);
    const U clamped = std::clamp(raw, MIN, MAX);

    dispatch(SetTrackBusType{parent->getSequenceIndex(), getIndex(),
                             static_cast<BusType>(clamped)});
}

BusType Track::getBusType() const
{
    return getSnapshot()->getBusType();
}

void Track::setDeviceIndex(const int i) const
{

    dispatch(SetTrackDeviceIndex{parent->getSequenceIndex(), getIndex(),
                                 static_cast<uint8_t>(std::clamp(i, 0, 32))});
}

int Track::getDeviceIndex() const
{
    return getSnapshot()->getDeviceIndex();
}

std::shared_ptr<Event> Track::getEvent(const int i) const
{
    const auto eventState = getSnapshot()->getEventByIndex(EventIndex(i));
    return mapEventStateToEvent(eventState, dispatch, parent);
}

void Track::setName(const std::string &s)
{
    name = s;
}

std::string Track::getName()
{
    if (!isUsed())
    {
        return "(Unused)";
    }
    return name;
}

std::vector<EventState> Track::getEventStates() const
{
    std::vector<EventState> result;
    for (const auto e : getSnapshot()->getEvents())
    {
        result.push_back(*e);
    }
    return result;
}

std::vector<std::shared_ptr<Event>> Track::getEvents() const
{
    std::vector<std::shared_ptr<Event>> result;

    const auto snapshot = getSnapshot();

    const int eventCount = snapshot->getEventCount();

    for (int i = 0; i < eventCount; ++i)
    {
        auto event = mapEventStateToEvent(
            snapshot->getEventByIndex(EventIndex(i)), dispatch, parent);
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
        if (const auto noteOff = bulkNoteOffs[noteOffIndex];
            noteOff->tick == TickUnassignedWhileRecording)
        {
            noteOff->tick = pos;
        }
    }

    for (int noteOnIndex = 0; noteOnIndex < noteOnCount; noteOnIndex++)
    {
        auto noteOn = bulkNoteOns[noteOnIndex];

        if (noteOn->tick == TickUnassignedWhileRecording)
        {
            noteOn->tick = pos;

            if (correctedTickPos != pos && correctedTickPos != -1)
            {
                noteOn->tick = correctedTickPos;
            }

            noteOn->wasMoved = noteOn->tick - pos;
        }

        bool needsToBeRequeued = true;

        for (int noteOffIndex = 0; noteOffIndex < noteOffCount; noteOffIndex++)
        {
            if (auto noteOff = bulkNoteOffs[noteOffIndex];
                noteOff->noteNumber == noteOn->noteNumber)
            {
                auto newTick = noteOff->tick + noteOn->wasMoved;
                if (newTick < 0)
                {
                    newTick += parent->getLastTick();
                }
                else if (newTick > parent->getLastTick())
                {
                    newTick -= parent->getLastTick();
                }

                noteOff->tick = newTick;

                auto duration = noteOff->tick - noteOn->tick;

                if (noteOff->tick < noteOn->tick)
                {
                    duration = parent->getLastTick() - noteOn->tick;
                }

                if (duration < 1)
                {
                    duration = 1;
                }

                noteOn->duration = Duration(duration);
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
    return getSnapshot()->isOn();
}

bool Track::isUsed() const
{
    return getSnapshot()->isUsed();
}

std::vector<std::shared_ptr<Event>>
Track::getEventRange(const int startTick, const int endTick) const
{
    std::vector<std::shared_ptr<Event>> result;
    for (const auto &e : getSnapshot()->getEventRange(startTick, endTick))
    {
        result.emplace_back(mapEventStateToEvent(e, dispatch, parent));
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

    for (const auto &event : getSnapshot()->getNoteEvents())
    {
        if (event->tick >= endPos)
        {
            break;
        }

        if (event->tick >= startPos && event->tick < endPos &&
            event->noteNumber >= lowestNote && event->noteNumber <= highestNote)
        {
            timingCorrect(fromBar, toBar, event, event->tick, stepLength,
                          swingPercentage);
        }
    }

    removeDoubles();
}

void Track::timingCorrect(const int fromBar, const int toBar, EventState *e,
                          const Tick eventTick, const int stepLength,
                          const int swingPercentage) const
{
    updateEventTick(e, timingCorrectTick(fromBar, toBar, eventTick, stepLength,
                                         swingPercentage));
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

void Track::updateEventTick(EventState *eventState, const int newTick) const
{
    dispatch(UpdateEventTick{eventState, newTick});
}

std::vector<std::shared_ptr<NoteOnEvent>> Track::getNoteEvents() const
{
    std::vector<std::shared_ptr<NoteOnEvent>> result;

    for (const auto &e : getSnapshot()->getNoteEvents())
    {
        result.emplace_back(std::dynamic_pointer_cast<NoteOnEvent>(
            mapEventStateToEvent(e, dispatch, parent)));
    }

    return result;
}

void Track::shiftTiming(EventState *e, const Tick eventTick, const bool later,
                        const int amount, const int lastTick) const
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

    updateEventTick(e, newEventTick);
}

std::string Track::getActualName()
{
    return name;
}

int Track::getNextTick()
{
    const auto snapshot = getSnapshot();
    const auto playEventIndex = snapshot->getPlayEventIndex();

    if (playEventIndex >= snapshot->getEventCount())
    {
        processRealtimeQueuedEvents();
        return std::numeric_limits<int>::max();
    }

    processRealtimeQueuedEvents();
    return snapshot->getEventByIndex(playEventIndex)->tick;
}

void Track::playNext() const
{
    const auto snapshot = getSnapshot();
    const auto eventCount = snapshot->getEventCount();
    auto playEventIndex = snapshot->getPlayEventIndex();

    if (playEventIndex >= eventCount)
    {
        return;
    }

    const auto event = snapshot->getEventByIndex(playEventIndex);

    const auto recordingModeIsMulti = isRecordingModeMulti();
    const auto isActiveTrackIndex = trackIndex == getActiveTrackIndex();
    auto _delete = isRecording() &&
                   (isActiveTrackIndex || recordingModeIsMulti) &&
                   trackIndex < 64;

    const auto pos = getTickPosition();

    if (isRecording() && isPunchEnabled() && trackIndex < 64)
    {
        _delete = false;

        if (getAutoPunchMode() == 0 && pos >= getPunchInTime())
        {
            _delete = true;
        }

        if (getAutoPunchMode() == 1 && pos < getPunchOutTime())
        {
            _delete = true;
        }

        if (getAutoPunchMode() == 2 && pos >= getPunchInTime() &&
            pos < getPunchOutTime())
        {
            _delete = true;
        }
    }

    if (event->type == EventType::NoteOn)
    {
        if (const auto drumBus = std::dynamic_pointer_cast<DrumBus>(
                getSequencerBus(getBusType()));
            drumBus && isOverdubbing() && isEraseButtonPressed() &&
            (isActiveTrackIndex || recordingModeIsMulti) && trackIndex < 64 &&
            drumBus)
        {
            const auto programIndex = drumBus->getProgramIndex();
            const auto program = sampler->getProgram(programIndex);

            const auto noteNumber = event->noteNumber;

            bool oneOrMorePadsArePressed = false;
            bool noteIsPressed = false;

            for (int programPadIndex = 0; programPadIndex < 64;
                 ++programPadIndex)
            {
                if (isProgramPadPressed(ProgramPadIndex(programPadIndex),
                                        programIndex))
                {
                    oneOrMorePadsArePressed = true;

                    if (program->getNoteFromPad(
                            ProgramPadIndex(programPadIndex)) == noteNumber)
                    {
                        noteIsPressed = true;
                        break;
                    }
                }
            }

            if (!_delete && oneOrMorePadsArePressed && isSixteenLevelsEnabled())
            {
                const auto vmpcSettingsScreen =
                    getScreens()->get<ScreenId::VmpcSettingsScreen>();
                const auto assign16LevelsScreen =
                    getScreens()->get<ScreenId::Assign16LevelsScreen>();

                if (vmpcSettingsScreen->_16LevelsEraseMode == 0)
                {
                    _delete = true;
                }
                else if (vmpcSettingsScreen->_16LevelsEraseMode == 1)
                {
                    const auto varValue = event->noteVariationValue;
                    const auto _16l_key =
                        assign16LevelsScreen->getOriginalKeyPad();
                    const auto _16l_type = assign16LevelsScreen->getType();

                    for (int programPadIndex = 0; programPadIndex < 64;
                         ++programPadIndex)
                    {
                        if (!isProgramPadPressed(
                                ProgramPadIndex(programPadIndex), programIndex))
                        {
                            continue;
                        }

                        int wouldBeVarValue;
                        const int padIndexWithoutBank = programPadIndex % 16;

                        if (_16l_type == 0)
                        {
                            const auto diff = padIndexWithoutBank - _16l_key;
                            auto candidate = 64 + diff * 5;

                            if (candidate > 124)
                            {
                                candidate = 124;
                            }
                            else if (candidate < 4)
                            {
                                candidate = 4;
                            }

                            wouldBeVarValue = candidate;
                        }
                        else
                        {
                            wouldBeVarValue = static_cast<int>(
                                floor(100 / 16.0) * padIndexWithoutBank);
                        }

                        if (varValue == wouldBeVarValue)
                        {
                            _delete = true;
                        }
                    }
                }
            }

            if (!_delete && noteIsPressed)
            {
                _delete = true;
            }
        }
    }

    if (_delete)
    {
        dispatch(RemoveEvent{parent->getSequenceIndex(), getIndex(), event});
        manager->drainQueue();
        return;
    }

    if (isOn() && (!isSoloEnabled() || getActiveTrackIndex() == trackIndex))
    {
        eventHandler->handleFinalizedEvent(*event, this);
    }

    playEventIndex = playEventIndex + 1;
    dispatch(SetPlayEventIndex{parent->getSequenceIndex(), getIndex(),
                               playEventIndex});
    manager->drainQueue();
}

void Track::insertEvent(
    EventState *event, const bool allowMultipleNoteEventsWithSameNoteOnSameTick,
    const std::function<void()> &onComplete)
{
    if (!isUsed())
    {
        setUsed(true);
    }

    event->sequenceIndex = parent->getSequenceIndex();
    event->trackIndex = trackIndex;

    dispatch(InsertEvent{event, allowMultipleNoteEventsWithSameNoteOnSameTick,
                         onComplete});
}

void Track::insertEvent(
    const EventState &eventState,
    const bool allowMultipleNoteEventsWithSameNoteOnSameTick,
    const std::function<void()> &onComplete)
{
    const auto e = manager->acquireEvent();
    *e = eventState;
    insertEvent(e, allowMultipleNoteEventsWithSameNoteOnSameTick, onComplete);
}
