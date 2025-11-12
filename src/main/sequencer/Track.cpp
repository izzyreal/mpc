#include "sequencer/Track.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Event.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "audiomidi/EventHandler.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include "lcdgui/screens/VmpcSettingsScreen.hpp"

#include <concurrentqueue.h>

#include <memory>

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
    const std::function<std::shared_ptr<Bus>(int)> &getSequencerBus,
    const std::function<bool()> &isEraseButtonPressed,
    const std::function<bool(int, ProgramIndex)> &isProgramPadPressed,
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
    busNumber = 1;
    bulkNoteOns.reserve(20);
    bulkNoteOffs.reserve(20);
    queuedNoteOnEvents = std::make_shared<
        moodycamel::ConcurrentQueue<std::shared_ptr<NoteOnEvent>>>(20);
    queuedNoteOffEvents = std::make_shared<
        moodycamel::ConcurrentQueue<std::shared_ptr<NoteOffEvent>>>(20);
}

std::shared_ptr<NoteOnEvent>
Track::findRecordingNoteOnEventById(const NoteEventId id)
{
    for (auto &e : events)
    {
        if (auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(e);
            noteOnEvent && noteOnEvent->getId() == id)
        {
            assert(noteOnEvent->isBeingRecorded());
            return noteOnEvent;
        }
    }
    std::shared_ptr<NoteOnEvent> found;
    std::shared_ptr<NoteOnEvent> e;
    bulkNoteOns.clear();
    while (queuedNoteOnEvents->try_dequeue(e))
    {
        if (e->getId() == id)
        {
            found = e;
        }
        assert(found->isBeingRecorded());
        bulkNoteOns.push_back(e);
    }

    for (auto &e2 : bulkNoteOns)
    {
        queuedNoteOnEvents->enqueue(e2);
    }

    bulkNoteOns.clear();

    return found;
}

std::shared_ptr<NoteOnEvent>
Track::findRecordingNoteOnEventByNoteNumber(const NoteNumber noteNumber)
{
    for (auto &e : events)
    {
        if (auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(e);
            noteOnEvent && noteOnEvent->isBeingRecorded() &&
            noteOnEvent->getNote() == noteNumber)
        {
            return noteOnEvent;
        }
    }

    std::shared_ptr<NoteOnEvent> found;
    std::shared_ptr<NoteOnEvent> e;
    bulkNoteOns.clear();
    while (queuedNoteOnEvents->try_dequeue(e))
    {
        if (e->getNote() == noteNumber && e->isBeingRecorded())
        {
            found = e;
        }
        bulkNoteOns.push_back(e);
    }

    for (auto &e2 : bulkNoteOns)
    {
        queuedNoteOnEvents->enqueue(e2);
    }

    bulkNoteOns.clear();

    return found;
}

void Track::move(const int tick, const int oldTick)
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
                                                 const int note) const
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

void Track::setTrackIndex(const int i)
{
    trackIndex = i;
}

int Track::getIndex() const
{
    return trackIndex;
}

void Track::flushNoteCache() const
{
    std::shared_ptr<NoteOnEvent> e1;
    std::shared_ptr<NoteOffEvent> e2;
    while (queuedNoteOnEvents->try_dequeue(e1))
    {
    }
    while (queuedNoteOffEvents->try_dequeue(e2))
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

std::shared_ptr<NoteOnEvent> Track::recordNoteEventLive(unsigned char note,
                                                        unsigned char velocity)
{
    auto noteOnEvent =
        std::make_shared<NoteOnEvent>(note, velocity, nextNoteEventId);
    noteOnEvent->setBeingRecorded(true);
    nextNoteEventId = getNextNoteEventId(nextNoteEventId);
    noteOnEvent->setTrack(getIndex());
    noteOnEvent->setTick(TickUnassignedWhileRecording);
    queuedNoteOnEvents->enqueue(noteOnEvent);
    return noteOnEvent;
}

void Track::finalizeNoteEventLive(
    const std::shared_ptr<NoteOnEvent> &event) const
{
    const auto offEvent = event->getNoteOff();
    offEvent->setTick(TickUnassignedWhileRecording);
    event->setBeingRecorded(false);
    queuedNoteOffEvents->enqueue(offEvent);
}

std::shared_ptr<NoteOnEvent>
Track::recordNoteEventNonLive(const int tick, int note, int velocity)
{
    auto onEvent = getNoteEvent(tick, note);
    if (!onEvent)
    {
        onEvent =
            std::make_shared<NoteOnEvent>(note, velocity, nextNoteEventId);
        nextNoteEventId = getNextNoteEventId(nextNoteEventId);
        onEvent->setTrack(this->getIndex());
        onEvent->setTick(tick);
        onEvent->setBeingRecorded(true);
        insertEventWhileRetainingSort(onEvent);
        return onEvent;
    }
    onEvent->setBeingRecorded(true);
    onEvent->setVelocity(velocity);
    onEvent->resetDuration();
    return onEvent;
}

void Track::addEvent(const int tick, const std::shared_ptr<Event> &event,
                     const bool allowMultipleNoteEventsWithSameNoteOnSameTick)
{
    if (events.empty())
    {
        setUsed(true);
    }

    event->setTick(tick);

    insertEventWhileRetainingSort(
        event, allowMultipleNoteEventsWithSameNoteOnSameTick);
}

void Track::cloneEventIntoTrack(std::shared_ptr<Event> &src, const int tick,
                                const bool allowMultipleNotesOnSameTick)
{
    std::shared_ptr<Event> clone;

    if (const auto source = std::dynamic_pointer_cast<NoteOnEvent>(src))
    {
        clone = std::make_shared<NoteOnEvent>(*source);
    }
    else if (const auto source = std::dynamic_pointer_cast<MixerEvent>(src))
    {
        clone = std::make_shared<MixerEvent>(*source);
    }
    else if (const auto source =
                 std::dynamic_pointer_cast<ChannelPressureEvent>(src))
    {
        clone = std::make_shared<ChannelPressureEvent>(*source);
    }
    else if (const auto source =
                 std::dynamic_pointer_cast<PolyPressureEvent>(src))
    {
        clone = std::make_shared<PolyPressureEvent>(*source);
    }
    else if (const auto source = std::dynamic_pointer_cast<PitchBendEvent>(src))
    {
        clone = std::make_shared<PitchBendEvent>(*source);
    }
    else if (const auto source =
                 std::dynamic_pointer_cast<TempoChangeEvent>(src))
    {
        const auto t = std::make_shared<TempoChangeEvent>(*source);
        t->setParent(parent);
        clone = t;
    }
    else if (const auto source =
                 std::dynamic_pointer_cast<ControlChangeEvent>(src))
    {
        clone = std::make_shared<ControlChangeEvent>(*source);
    }
    else if (const auto source =
                 std::dynamic_pointer_cast<ProgramChangeEvent>(src))
    {
        clone = std::make_shared<ProgramChangeEvent>(*source);
    }
    else if (const auto source =
                 std::dynamic_pointer_cast<SystemExclusiveEvent>(src))
    {
        clone = std::make_shared<SystemExclusiveEvent>(*source);
    }
    clone->setTick(tick);

    if (!used)
    {
        setUsed(true);
    }

    insertEventWhileRetainingSort(clone, allowMultipleNotesOnSameTick);
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
    if (i < 0 || i > 128)
    {
        return;
    }

    programChange = i;
}

int Track::getProgramChange() const
{
    return programChange;
}

void Track::setBusNumber(const int i)
{
    if (i < 0 || i > 4)
    {
        return;
    }

    busNumber = i;
}

int Track::getBus() const
{
    return busNumber;
}

void Track::setDeviceIndex(const int i)
{
    if (i < 0 || i > 32)
    {
        return;
    }

    device = i;
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

        const auto lastTick = parent->getLastTick();

        if (shiftedTick > lastTick)
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
        this->queuedNoteOnEvents->try_dequeue_bulk(bulkNoteOns.begin(), 20);
    const auto noteOffCount =
        this->queuedNoteOffEvents->try_dequeue_bulk(bulkNoteOffs.begin(), 20);

    if (noteOnCount == 0 && noteOffCount == 0)
    {
        return;
    }

    const auto pos = getTickPosition();
    const auto correctedTickPos = getCorrectedTickPos();

    for (int noteOffIndex = 0; noteOffIndex < noteOffCount; noteOffIndex++)
    {
        const auto noteOff = bulkNoteOffs[noteOffIndex];

        if (noteOff->getTick() == TickUnassignedWhileRecording)
        {
            noteOff->setTick(pos);
        }
    }

    for (int noteOnIndex = 0; noteOnIndex < noteOnCount; noteOnIndex++)
    {
        auto noteOn = bulkNoteOns[noteOnIndex];

        if (noteOn->getTick() == TickUnassignedWhileRecording)
        {
            noteOn->setTick(pos);

            if (correctedTickPos != pos && correctedTickPos != -1)
            {
                noteOn->setTick(correctedTickPos);
            }

            noteOn->wasMoved = noteOn->getTick() - pos;
        }

        bool needsToBeRequeued = true;

        for (int noteOffIndex = 0; noteOffIndex < noteOffCount; noteOffIndex++)
        {
            auto noteOff = bulkNoteOffs[noteOffIndex];

            if (noteOff->getNote() == noteOn->getNote())
            {
                auto newTick = noteOff->getTick() + noteOn->wasMoved;
                if (newTick < 0)
                {
                    newTick += parent->getLastTick();
                }
                else if (newTick > parent->getLastTick())
                {
                    newTick -= parent->getLastTick();
                }
                noteOff->setTick(newTick);

                auto duration = noteOff->getTick() - noteOn->getTick();
                bool fixEventIndex = false;

                if (noteOff->getTick() < noteOn->getTick())
                {
                    duration = parent->getLastTick() - noteOn->getTick();
                    fixEventIndex = true;
                    noteOn->dontDelete = true;
                }

                if (duration < 1)
                {
                    duration = 1;
                }
                noteOn->setDuration(duration);

                const bool wasInserted = insertEventWhileRetainingSort(noteOn);

                if (fixEventIndex && wasInserted)
                {
                    eventIndex--;
                }

                needsToBeRequeued = false;
            }
            else
            {
                this->queuedNoteOffEvents->enqueue(noteOff);
            }
        }

        if (needsToBeRequeued)
        {
            this->queuedNoteOnEvents->enqueue(noteOn);
        }
    }
}

int Track::getNextTick()
{
    if (eventIndex >= events.size())
    {
        processRealtimeQueuedEvents();
        return MAX_TICK;
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
                std::dynamic_pointer_cast<DrumBus>(getSequencerBus(busNumber));
            drumBus && isOverdubbing() && isEraseButtonPressed() &&
            (isActiveTrackIndex || recordingModeIsMulti) && trackIndex < 64 &&
            drumBus)
        {
            const auto programIndex = drumBus->getProgram();
            const auto program = sampler->getProgram(programIndex);

            const auto noteNumber = note->getNote();

            bool oneOrMorePadsArePressed = false;
            bool noteIsPressed = false;

            for (int programPadIndex = 0; programPadIndex < 64;
                 ++programPadIndex)
            {
                if (isProgramPadPressed(programPadIndex, programIndex))
                {
                    oneOrMorePadsArePressed = true;

                    if (program->getNoteFromPad(programPadIndex) == noteNumber)
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
                        if (!isProgramPadPressed(programPadIndex, programIndex))
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
        eventHandler->handleFinalizedEvent(event, this);
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
    auto event = std::dynamic_pointer_cast<Event>(noteEvent);
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
        const int stepEnd = i * stepLength + stepLength / 2;

        if (tick - currentBarStart >= stepStart &&
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
        auto ne = std::dynamic_pointer_cast<NoteOnEvent>(e);

        if (ne)
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

void Track::updateEventTick(std::shared_ptr<Event> &e, const int newTick)
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
        auto ne = std::dynamic_pointer_cast<NoteOnEvent>(e);

        if (ne)
        {
            noteEvents.push_back(ne);
        }
    }

    return noteEvents;
}

void Track::shiftTiming(std::shared_ptr<Event> event, const bool later,
                        int amount, const int lastTick)
{
    if (!later)
    {
        amount *= -1;
    }

    int newEventTick = event->getTick() + amount;

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

bool Track::insertEventWhileRetainingSort(
    const std::shared_ptr<Event> &event,
    const bool allowMultipleNoteEventsWithSameNoteOnSameTick)
{
    if (!isUsed())
    {
        setUsed(true);
    }

    auto tick = event->getTick();

    auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(event);

    if (noteEvent && !allowMultipleNoteEventsWithSameNoteOnSameTick)
    {
        for (auto &e : events)
        {
            if (auto e2 = std::dynamic_pointer_cast<NoteOnEvent>(e);
                e2 && e2->getTick() == tick &&
                e2->getNote() == noteEvent->getNote())
            {
                e2.swap(noteEvent);
                return false;
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
            events.emplace_back(event);
        }
        else
        {
            events.emplace(insertAt, event);
        }
    }
    else
    {
        events.emplace_back(event);
    }

    eventIndex++;

    return true;
}
