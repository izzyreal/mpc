#include "sequencer/Track.hpp"

#include "EventStateToEventMapper.hpp"
#include "sampler/Sampler.hpp"

#include "sequencer/TrackEventStateManager.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Event.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/NoteEvent.hpp"
#include "audiomidi/EventHandler.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include "lcdgui/screens/VmpcSettingsScreen.hpp"

#include <concurrentqueue.h>

#include <memory>
#include <limits>

using namespace mpc::sequencer;
using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

constexpr int TickUnassignedWhileRecording = -2;

Track::Track(
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
    : trackIndex(trackIndex), parent(parent),
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
    eventStateManager = std::make_shared<TrackEventStateManager>();
    purge();
}

void Track::purge()
{
    events.clear();
    name = trackIndex == 64 ? "tempo" : getDefaultTrackName(trackIndex);
    programChange = 0;
    velocityRatio = 100;
    used = false;
    on = true;
    eventIndex = 0;
    device = 0;
    busType = BusType::DRUM1;
    bulkNoteOns.resize(20);
    bulkNoteOffs.resize(20);
    queuedNoteOnEvents = std::make_shared<
        moodycamel::ConcurrentQueue<sequencer::EventState>>(20);
    queuedNoteOffEvents = std::make_shared<
        moodycamel::ConcurrentQueue<sequencer::EventState>>(20);
}

mpc::sequencer::EventState
Track::findRecordingNoteOnEventById(const NoteEventId id)
{
    for (auto &e : events)
    {
        if (const auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(e);
            noteOnEvent && noteOnEvent->getId() == id)
        {
            assert(noteOnEvent->isBeingRecorded());
            return noteOnEvent->getSnapshot();
        }
    }

    sequencer::EventState found;
    sequencer::EventState e;

    size_t count = 0;

    while (count < bulkNoteOns.size() && queuedNoteOnEvents->try_dequeue(e))
    {
        if (e.noteEventId == id)
        {
            found = e;
        }

        assert(e.beingRecorded);
        bulkNoteOns[count++] = e;
    }

    for (size_t i = 0; i < count; i++)
    {
        queuedNoteOnEvents->enqueue(bulkNoteOns[i]);
    }

    return found;
}

mpc::sequencer::EventState
Track::findRecordingNoteOnEventByNoteNumber(const NoteNumber noteNumber)
{
    for (auto &e : events)
    {
        if (const auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(e);
            noteOnEvent && noteOnEvent->isBeingRecorded() &&
            noteOnEvent->getNote() == noteNumber)
        {
            return noteOnEvent->getSnapshot();
        }
    }

    sequencer::EventState found;
    sequencer::EventState e;

    size_t count = 0;

    while (count < bulkNoteOns.size() && queuedNoteOnEvents->try_dequeue(e))
    {
        if (e.beingRecorded && e.noteNumber == noteNumber)
        {
            found = e;
        }

        bulkNoteOns[count++] = e;
    }

    for (size_t i = 0; i < count; i++)
    {
        queuedNoteOnEvents->enqueue(bulkNoteOns[i]);
    }

    return found;
}

void Track::syncEventIndex(const int tick, const int oldTick)
{
    if (tick == 0)
    {
        eventIndex = 0;
        return;
    }

    auto startIndex = 0;

    if (tick > oldTick)
    {
        if (eventIndex == events.size())
        {
            return;
        }
        startIndex = eventIndex;
    }

    if (tick < oldTick && eventIndex == 0)
    {
        return;
    }

    eventIndex = events.size();

    for (int i = startIndex; i < events.size(); i++)
    {
        if (events[i]->getTick() >= tick)
        {
            eventIndex = i;
            break;
        }
    }
}

std::shared_ptr<NoteOnEvent> Track::getNoteEvent(const int tick,
                                                 const NoteNumber note) const
{
    for (auto &e : events)
    {
        if (const auto e2 = std::dynamic_pointer_cast<NoteOnEvent>(e);
            e2 && e2->getTick() == tick && e2->getNote() == note)
        {
            return e2;
        }
    }
    return {};
}

void Track::setTrackIndex(const TrackIndex i)
{
    trackIndex = i;
}

mpc::TrackIndex Track::getIndex() const
{
    return trackIndex;
}

void Track::flushNoteCache() const
{
    sequencer::EventState e;
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

void Track::removeEvent(const std::shared_ptr<Event> &event)
{
    for (int i = 0; i < events.size(); i++)
    {
        if (events[i] == event)
        {
            events.erase(events.begin() + i);
            break;
        }
    }
}

mpc::sequencer::EventState Track::recordNoteEventLive(const NoteNumber note,
                                                   const Velocity velocity)
{
    const NoteEventId noteEventIdToUse = nextNoteEventId;
    nextNoteEventId = getNextNoteEventId(nextNoteEventId);

    sequencer::EventState e;
    e.type = sequencer::EventType::NoteOn;
    e.noteNumber = note;
    e.velocity = velocity;
    e.noteEventId = noteEventIdToUse;
    e.beingRecorded = true;
    e.trackIndex = getIndex();
    e.tick = TickUnassignedWhileRecording;
    queuedNoteOnEvents->enqueue(e);
    return e;
}

void Track::finalizeNoteEventLive(const sequencer::EventState &noteOnEvent) const
{
    sequencer::EventState e;
    e.type = sequencer::EventType::NoteOff;
    e.noteNumber = noteOnEvent.noteNumber;
    e.tick = TickUnassignedWhileRecording;
    queuedNoteOffEvents->enqueue(e);
}

void Track::finalizeNoteEventNonLive(const sequencer::EventState &noteOnEvent) const
{
    sequencer::EventState e;
    e.type = sequencer::EventType::NoteOff;
    e.noteNumber = noteOnEvent.noteNumber;
    queuedNoteOffEvents->enqueue(e);
}

mpc::sequencer::EventState Track::recordNoteEventNonLive(const int tick,
                                                           const NoteNumber note,
                                                           const Velocity velocity,
                                                           const int64_t metronomeOnlyTick)
{
    return{};

    // auto onEvent = getNoteEvent(tick, note);
    //
    // if (!onEvent)
    // {
    //     onEvent =
    //         std::make_shared<NoteOnEvent>(note, velocity, nextNoteEventId);
    //     nextNoteEventId = getNextNoteEventId(nextNoteEventId);
    //     onEvent->setTrack(this->getIndex());
    //     onEvent->setTick(tick);
    //     onEvent->setBeingRecorded(true);
    //     insertEventWhileRetainingSort(onEvent);
    //     return onEvent;
    // }
    // onEvent->setBeingRecorded(true);
    // onEvent->setVelocity(velocity);
    // onEvent->resetDuration();
    // return onEvent;
}

void Track::addEvent(const sequencer::EventState &event,
                     const bool allowMultipleNoteEventsWithSameNoteOnSameTick)
{
    if (events.empty())
    {
        setUsed(true);
    }

    insertEventWhileRetainingSort(
        event, allowMultipleNoteEventsWithSameNoteOnSameTick);
}

void Track::cloneEventIntoTrack(const std::shared_ptr<Event> &src,
                                const int tick,
                                const bool allowMultipleNotesOnSameTick)
{
    std::shared_ptr<Event> clone;

    // if (const auto noteOnSrc = std::dynamic_pointer_cast<NoteOnEvent>(src))
    // {
    //     clone = std::make_shared<NoteOnEvent>(*noteOnSrc);
    // }
    // else if (const auto mixerSrc = std::dynamic_pointer_cast<MixerEvent>(src))
    // {
    //     clone = std::make_shared<MixerEvent>(*mixerSrc);
    // }
    // else if (const auto chanPressSrc =
    //              std::dynamic_pointer_cast<ChannelPressureEvent>(src))
    // {
    //     clone = std::make_shared<ChannelPressureEvent>(*chanPressSrc);
    // }
    // else if (const auto polyPressSrc =
    //              std::dynamic_pointer_cast<PolyPressureEvent>(src))
    // {
    //     clone = std::make_shared<PolyPressureEvent>(*polyPressSrc);
    // }
    // else if (const auto pitchBendSrc =
    //              std::dynamic_pointer_cast<PitchBendEvent>(src))
    // {
    //     clone = std::make_shared<PitchBendEvent>(*pitchBendSrc);
    // }
    // else if (const auto tempoSrc =
    //              std::dynamic_pointer_cast<TempoChangeEvent>(src))
    // {
    //     const auto t = std::make_shared<TempoChangeEvent>(*tempoSrc);
    //     t->setParent(parent);
    //     clone = t;
    // }
    // else if (const auto ctrlChangeSrc =
    //              std::dynamic_pointer_cast<ControlChangeEvent>(src))
    // {
    //     clone = std::make_shared<ControlChangeEvent>(*ctrlChangeSrc);
    // }
    // else if (const auto progChangeSrc =
    //              std::dynamic_pointer_cast<ProgramChangeEvent>(src))
    // {
    //     clone = std::make_shared<ProgramChangeEvent>(*progChangeSrc);
    // }
    // else if (const auto sysExSrc =
    //              std::dynamic_pointer_cast<SystemExclusiveEvent>(src))
    // {
    //     clone = std::make_shared<SystemExclusiveEvent>(*sysExSrc);
    // }
    //
    // if (clone)
    // {
    //     clone->setTick(tick);
    //
    //     if (!used)
    //     {
    //         setUsed(true);
    //     }
    //
    //     insertEventWhileRetainingSort(clone, allowMultipleNotesOnSameTick);
    // }
}
void Track::cloneEventIntoTrack(const sequencer::EventState &e,
                                const bool allowMultipleNotesOnSameTick)
{
    printf("cloning into track with tick %lld and duration %i\n", e.tick, e.duration.get());
    insertEventWhileRetainingSort(e, allowMultipleNotesOnSameTick);
}

void Track::removeEvent(const int i)
{
    events.erase(events.begin() + i);
}

void Track::removeEvents()
{
    events.clear();
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

std::shared_ptr<Event> Track::getEvent(const int i)
{
    return events[i];
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

std::vector<std::shared_ptr<Event>> &Track::getEvents()
{
    return events;
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
                bool fixEventIndex = false;

                if (noteOff.tick < noteOn.tick)
                {
                    duration = parent->getLastTick() - noteOn.tick;
                    fixEventIndex = true;
                    noteOn.dontDelete = true;
                }

                if (duration < 1)
                {
                    duration = 1;
                }

                noteOn.duration = Duration(duration);
                insertEventWhileRetainingSort(noteOn);

                if (fixEventIndex)
                {
                    eventIndex--;
                }

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

int Track::getNextTick()
{
    if (eventIndex >= events.size())
    {
        processRealtimeQueuedEvents();
        return std::numeric_limits<int>::max();
    }

    processRealtimeQueuedEvents();
    return events[eventIndex]->getTick();
}

void Track::playNext()
{
    if (eventIndex >= events.size())
    {
        return;
    }

    const auto recordingModeIsMulti = isRecordingModeMulti();
    const auto isActiveTrackIndex = trackIndex == getActiveTrackIndex();
    auto _delete = isRecording() &&
                   (isActiveTrackIndex || recordingModeIsMulti) &&
                   trackIndex < 64;

    if (isRecording() && isPunchEnabled() && trackIndex < 64)
    {
        const auto pos = getTickPosition();

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

    const auto event = eventIndex >= events.size() ? std::shared_ptr<Event>()
                                                   : events[eventIndex];
    if (const auto note = std::dynamic_pointer_cast<NoteOnEvent>(event))
    {
        note->setTrack(trackIndex);

        if (const auto drumBus =
                std::dynamic_pointer_cast<DrumBus>(getSequencerBus(busType));
            drumBus && isOverdubbing() && isEraseButtonPressed() &&
            (isActiveTrackIndex || recordingModeIsMulti) && trackIndex < 64 &&
            drumBus)
        {
            const auto programIndex = drumBus->getProgramIndex();
            const auto program = sampler->getProgram(programIndex);

            const auto noteNumber = note->getNote();

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
                    const auto &varValue = note->getVariationValue();
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

    if (_delete && !events[eventIndex]->dontDelete)
    {
        events.erase(events.begin() + eventIndex);
        return;
    }

    events[eventIndex]->dontDelete = false;

    if (isOn() && (!isSoloEnabled() || getActiveTrackIndex() == trackIndex))
    {
        eventHandler->handleFinalizedEvent(event->getSnapshot(), this);
    }

    eventIndex++;
}

bool Track::isOn() const
{
    return on;
}

bool Track::isUsed() const
{
    return used || !events.empty();
}

std::vector<std::shared_ptr<Event>>
Track::getEventRange(const int startTick, const int endTick) const
{
    std::vector<std::shared_ptr<Event>> res;

    for (auto &e : events)
    {
        if (e->getTick() >= startTick && e->getTick() <= endTick)
        {
            res.push_back(e);
        }
    }

    return res;
}

void Track::correctTimeRange(const int startPos, const int endPos,
                             const int stepLength, const int swingPercentage,
                             const int lowestNote, const int highestNote)
{
    const auto s = getActiveSequence();
    int accumBarLengths = 0;
    auto fromBar = 0;
    auto toBar = 0;

    for (int i = 0; i < 999; i++)
    {
        accumBarLengths += s->getBarLengthsInTicks()[i];

        if (accumBarLengths >= startPos)
        {
            fromBar = i;
            break;
        }
    }

    for (int i = 0; i < 999; i++)
    {
        accumBarLengths += s->getBarLengthsInTicks()[i];

        if (accumBarLengths > endPos)
        {
            toBar = i;
            break;
        }
    }

    for (auto &event : events)
    {
        if (auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(event))
        {
            if (noteEvent->getTick() >= endPos)
            {
                break;
            }

            if (noteEvent->getTick() >= startPos &&
                noteEvent->getTick() < endPos &&
                noteEvent->getNote() >= lowestNote &&
                noteEvent->getNote() <= highestNote)
            {
                timingCorrect(fromBar, toBar, noteEvent, stepLength,
                              swingPercentage);
            }
        }
    }

    removeDoubles();
}

void Track::timingCorrect(const int fromBar, const int toBar,
                          const std::shared_ptr<NoteOnEvent> &noteEvent,
                          const int stepLength, const int swingPercentage)
{
    const auto event = std::dynamic_pointer_cast<Event>(noteEvent);
    updateEventTick(event,
                    timingCorrectTick(fromBar, toBar, noteEvent->getTick(),
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
    const auto sequence = getActiveSequence();
    int segmentStart = 0;
    int segmentEnd = 0;

    for (int i = 0; i < 999; i++)
    {
        if (i < fromBar)
        {
            segmentStart += sequence->getBarLengthsInTicks()[i];
        }

        if (i <= toBar)
        {
            segmentEnd += sequence->getBarLengthsInTicks()[i];
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < 999; i++)
    {
        accumBarLengths += sequence->getBarLengthsInTicks()[i];

        if (tick < accumBarLengths && tick >= previousAccumBarLengths)
        {
            barNumber = i;
            break;
        }

        previousAccumBarLengths = accumBarLengths;
    }

    for (int i = 1; i < 1000; i++)
    {
        if (sequence->getBarLengthsInTicks()[barNumber] - i * stepLength < 0)
        {
            numberOfSteps = i - 1;
            break;
        }
    }

    int currentBarStart = 0;

    for (int i = 0; i < barNumber; i++)
    {
        currentBarStart += sequence->getBarLengthsInTicks()[i];
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

void Track::removeDoubles()
{
    auto eventCounter = 0;
    std::vector<int> deleteIndexList;
    std::vector<int> notesAtTick;
    int lastTick = -100;

    for (auto &e : events)
    {
        if (const auto ne = std::dynamic_pointer_cast<NoteOnEvent>(e))
        {
            if (lastTick != e->getTick())
            {
                notesAtTick.clear();
            }

            bool contains = false;

            for (const auto &n : notesAtTick)
            {
                if (n == ne->getNote())
                {
                    contains = true;
                    break;
                }
            }

            if (!contains)
            {
                notesAtTick.push_back(ne->getNote());
            }
            else
            {
                deleteIndexList.push_back(eventCounter);
            }

            lastTick = e->getTick();
        }
        eventCounter++;
    }

    reverse(deleteIndexList.begin(), deleteIndexList.end());

    for (const auto &i : deleteIndexList)
    {
        events.erase(events.begin() + i);
    }
}

template <typename t>
void moveEvent(std::vector<t> &v, size_t oldIndex, size_t newIndex)
{
    if (oldIndex > newIndex)
    {
        std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex,
                    v.rend() - newIndex);
    }
    else
    {
        std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1,
                    v.begin() + newIndex + 1);
    }
}

void Track::updateEventTick(const std::shared_ptr<Event> &e, const int newTick)
{
    if (e->getTick() == newTick)
    {
        return;
    }

    int currentIndex = -1;
    int newIndex = -1;

    bool higherOneExists = false;

    for (int i = 0; i < events.size(); i++)
    {
        if (currentIndex >= 0 && newIndex >= 0)
        {
            break;
        }

        if (events[i] == e)
        {
            currentIndex = i;
            continue;
        }

        if (newIndex == -1 && events[i]->getTick() > newTick)
        {
            higherOneExists = true;
            newIndex = i - 1;
        }
    }

    if (!higherOneExists)
    {
        newIndex = events.size() - 1;
    }

    moveEvent(events, currentIndex, newIndex);

    e->setTick(newTick);
}

std::vector<std::shared_ptr<NoteOnEvent>> Track::getNoteEvents() const
{
    std::vector<std::shared_ptr<NoteOnEvent>> noteEvents;

    for (auto &e : events)
    {
        if (auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(e))
        {
            noteEvents.push_back(noteEvent);
        }
    }

    return noteEvents;
}

void Track::shiftTiming(const std::shared_ptr<Event> &event, const bool later,
                        const int amount, const int lastTick)
{
    int amountToUse = amount;

    if (!later)
    {
        amountToUse *= -1;
    }

    int newEventTick = event->getTick() + amountToUse;

    if (newEventTick < 0)
    {
        newEventTick = 0;
    }
    else if (newEventTick > lastTick)
    {
        newEventTick = lastTick;
    }

    updateEventTick(event, newEventTick);
}

std::string Track::getActualName()
{
    return name;
}

void Track::insertEventWhileRetainingSort(
    const sequencer::EventState &event,
    const bool allowMultipleNoteEventsWithSameNoteOnSameTick)
{
    if (!isUsed())
    {
        setUsed(true);
    }

    auto tick = event.tick;

    if (event.type == sequencer::EventType::NoteOn &&
        !allowMultipleNoteEventsWithSameNoteOnSameTick)
    {
        for (auto it = events.begin(); it != events.end(); ++it)
        {
            if (const auto n = std::dynamic_pointer_cast<NoteOnEvent>(*it))
            {
                if (n->getTick() == tick && n->getNote() == event.noteNumber)
                {
                    events.erase(it);
                    break;
                }
            }
        }
    }

    const bool insertRequired =
        !events.empty() && events.back()->getTick() >= tick;

    if (insertRequired)
    {
        const auto insertAt =
            std::find_if(events.begin(), events.end(),
                         [&tick](const std::shared_ptr<Event> &e)
                         {
                             return e->getTick() > tick;
                         });

        if (insertAt == events.end())
        {
            events.emplace_back(mapEventStateToEvent(event));
        }
        else
        {
            events.emplace(insertAt, mapEventStateToEvent(event));
        }
    }
    else
    {
        events.emplace_back(mapEventStateToEvent(event));
    }

    eventIndex++;
}
