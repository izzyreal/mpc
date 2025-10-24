#include <iostream>
#include "sequencer/Track.hpp"

#include <Mpc.hpp>
#include "audiomidi/EventHandler.hpp"

#include "sequencer/Event.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/MidiClockEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"

#include "lcdgui/screens/PunchScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include "lcdgui/screens/VmpcSettingsScreen.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/Component.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

using namespace mpc::sequencer;

Track::Track(mpc::Mpc &mpc, Sequence *parent, int i)
    : mpc(mpc)
{
    this->parent = parent;

    trackIndex = i;
    programChange = 0;
    velocityRatio = 100;
    used = false;
    on = true;
    eventIndex = 0;
    device = 0;
    busNumber = 1;
}

void Track::move(int tick, int oldTick)
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

std::shared_ptr<NoteOnEvent> Track::getNoteEvent(int tick, int note)
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

void Track::setTrackIndex(int i)
{
    trackIndex = i;
}

int Track::getIndex()
{
    return trackIndex;
}

void Track::flushNoteCache()
{
    std::shared_ptr<NoteOnEvent> e1;
    std::shared_ptr<NoteOffEvent> e2;
    while (queuedNoteOnEvents.try_dequeue(e1))
    {
    }
    while (queuedNoteOffEvents.try_dequeue(e2))
    {
    }
}

void Track::setUsed(bool b)
{
    if (!used && b && trackIndex < 64)
    {
        name = mpc.getSequencer()->getDefaultTrackName(trackIndex);
    }

    used = b;

    if (used)
    {
        notifyObservers(std::string("tracknumbername"));
    }
}

void Track::setOn(bool b)
{
    on = b;

    notifyObservers(std::string("trackon"));
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

    notifyObservers(std::string("step-editor"));
}

// This is called from the UI thread. Results in incorrect tickpos.
// We should only queue the fact that a note of n wants to be recorded.
// Then we let getNextTick, from the audio thread, set the tickpos.
std::shared_ptr<NoteOnEvent> Track::recordNoteEventASync(unsigned char note, unsigned char velocity)
{
    auto onEvent = std::make_shared<NoteOnEvent>(note, velocity);
    onEvent->setTrack(getIndex());
    onEvent->setTick(-2);
    queuedNoteOnEvents.enqueue(onEvent);
    return onEvent;
}

void Track::finalizeNoteEventASync(const std::shared_ptr<NoteOnEvent> &event)
{
    auto offEvent = event->getNoteOff();
    offEvent->setTick(-2);
    queuedNoteOffEvents.enqueue(offEvent);
}

std::shared_ptr<NoteOnEvent> Track::recordNoteEventSynced(int tick, int note, int velocity)
{
    auto onEvent = getNoteEvent(tick, note);
    if (!onEvent)
    {
        onEvent = std::make_shared<NoteOnEvent>(note, velocity);
        onEvent->setTrack(this->getIndex());
        onEvent->setTick(tick);
        insertEventWhileRetainingSort(onEvent);
        notifyObservers(std::string("step-editor"));
        return onEvent;
    }
    else
    {
        onEvent->setVelocity(velocity);
        onEvent->resetDuration();
        return onEvent;
    }
}

bool Track::finalizeNoteEventSynced(const std::shared_ptr<NoteOnEvent> &event, int duration)
{
    auto old_duration = event->getDuration();
    event->setDuration(duration);
    notifyObservers(std::string("adjust-duration"));
    return old_duration != duration;
}

void Track::addEvent(int tick, const std::shared_ptr<Event> &event, bool allowMultipleNoteEventsWithSameNoteOnSameTick)
{
    if (events.empty())
    {
        setUsed(true);
    }

    event->setTick(tick);

    insertEventWhileRetainingSort(event, allowMultipleNoteEventsWithSameNoteOnSameTick);

    notifyObservers(std::string("step-editor"));
}

void Track::cloneEventIntoTrack(std::shared_ptr<Event> &src, int tick, bool allowMultipleNotesOnSameTick)
{
    std::shared_ptr<Event> clone;

    if (auto source = std::dynamic_pointer_cast<NoteOnEvent>(src))
    {
        clone = std::make_shared<NoteOnEvent>(*source);
    }
    else if (auto source = std::dynamic_pointer_cast<MidiClockEvent>(src))
    {
        clone = std::make_shared<MidiClockEvent>(*source);
    }
    else if (auto source = std::dynamic_pointer_cast<MixerEvent>(src))
    {
        clone = std::make_shared<MixerEvent>(*source);
    }
    else if (auto source = std::dynamic_pointer_cast<ChannelPressureEvent>(src))
    {
        clone = std::make_shared<ChannelPressureEvent>(*source);
    }
    else if (auto source = std::dynamic_pointer_cast<PolyPressureEvent>(src))
    {
        clone = std::make_shared<PolyPressureEvent>(*source);
    }
    else if (auto source = std::dynamic_pointer_cast<PitchBendEvent>(src))
    {
        clone = std::make_shared<PitchBendEvent>(*source);
    }
    else if (auto source = std::dynamic_pointer_cast<TempoChangeEvent>(src))
    {
        auto t = std::make_shared<TempoChangeEvent>(*source);
        t->setParent(parent);
        clone = t;
    }
    else if (auto source = std::dynamic_pointer_cast<ControlChangeEvent>(src))
    {
        clone = std::make_shared<ControlChangeEvent>(*source);
    }
    else if (auto source = std::dynamic_pointer_cast<ProgramChangeEvent>(src))
    {
        clone = std::make_shared<ProgramChangeEvent>(*source);
    }
    else if (auto source = std::dynamic_pointer_cast<SystemExclusiveEvent>(src))
    {
        clone = std::make_shared<SystemExclusiveEvent>(*source);
    }
    clone->setTick(tick);

    if (!used)
    {
        setUsed(true);
    }

    insertEventWhileRetainingSort(clone, allowMultipleNotesOnSameTick);
    notifyObservers(std::string("step-editor"));
}

void Track::removeEvent(int i)
{
    events.erase(events.begin() + i);

    notifyObservers(std::string("step-editor"));
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

    notifyObservers(std::string("velocityratio"));
}

int Track::getVelocityRatio()
{
    return velocityRatio;
}

void Track::setProgramChange(int i)
{
    if (i < 0 || i > 128)
    {
        return;
    }

    programChange = i;

    notifyObservers(std::string("programchange"));
}

int Track::getProgramChange()
{
    return programChange;
}

void Track::setBusNumber(int i)
{
    if (i < 0 || i > 4)
    {
        return;
    }

    busNumber = i;

    notifyObservers(std::string("bus"));
}

int Track::getBus()
{
    return busNumber;
}

void Track::setDeviceIndex(int i)
{
    if (i < 0 || i > 32)
    {
        return;
    }

    device = i;

    notifyObservers(std::string("device"));

    notifyObservers(std::string("devicename"));
}

int Track::getDeviceIndex()
{
    return device;
}

std::shared_ptr<Event> Track::getEvent(int i)
{
    return events[i];
}

void Track::setName(std::string s)
{
    name = s;

    notifyObservers(std::string("tracknumbername"));
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

int Track::getCorrectedTickPos()
{
    auto pos = mpc.getSequencer()->getTickPosition();
    auto correctedTickPos = -1;

    auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>();
    auto swingPercentage = timingCorrectScreen->getSwing();
    auto noteValueLengthInTicks = timingCorrectScreen->getNoteValueLengthInTicks();

    if (noteValueLengthInTicks > 1)
    {
        correctedTickPos = timingCorrectTick(0, parent->getLastBarIndex(), pos, noteValueLengthInTicks, swingPercentage);
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

        auto lastTick = parent->getLastTick();

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
    auto noteOnCount = this->queuedNoteOnEvents.try_dequeue_bulk(bulkNoteOns.begin(), 20);
    auto noteOffCount = this->queuedNoteOffEvents.try_dequeue_bulk(bulkNoteOffs.begin(), 20);

    if (noteOnCount == 0 && noteOffCount == 0)
    {
        return;
    }

    auto pos = mpc.getSequencer()->getTickPosition();
    auto correctedTickPos = getCorrectedTickPos();

    for (int noteOffIndex = 0; noteOffIndex < noteOffCount; noteOffIndex++)
    {
        auto noteOff = bulkNoteOffs[noteOffIndex];

        if (noteOff->getTick() == -2)
        {
            noteOff->setTick(pos);
        }
    }

    for (int noteOnIndex = 0; noteOnIndex < noteOnCount; noteOnIndex++)
    {

        auto noteOn = bulkNoteOns[noteOnIndex];

        if (noteOn->getTick() == -2)
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

                bool wasInserted = insertEventWhileRetainingSort(noteOn);

                if (fixEventIndex && wasInserted)
                {
                    eventIndex--;
                }

                needsToBeRequeued = false;
            }
            else
            {
                this->queuedNoteOffEvents.enqueue(noteOff);
            }
        }

        if (needsToBeRequeued)
        {
            this->queuedNoteOnEvents.enqueue(noteOn);
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

    const auto sequencer = mpc.getSequencer();

    const auto recordingModeIsMulti = sequencer->isRecordingModeMulti();
    const auto isActiveTrackIndex = trackIndex == sequencer->getActiveTrackIndex();
    auto _delete = sequencer->isRecording() && (isActiveTrackIndex || recordingModeIsMulti) && (trackIndex < 64);

    auto punchScreen = mpc.screens->get<PunchScreen>();

    if (sequencer->isRecording() && punchScreen->on && trackIndex < 64)
    {
        auto pos = sequencer->getTickPosition();

        _delete = false;

        if (punchScreen->autoPunch == 0 && pos >= punchScreen->time0)
        {
            _delete = true;
        }

        if (punchScreen->autoPunch == 1 && pos < punchScreen->time1)
        {
            _delete = true;
        }

        if (punchScreen->autoPunch == 2 && pos >= punchScreen->time0 && pos < punchScreen->time1)
        {
            _delete = true;
        }
    }

    const auto event = eventIndex >= events.size() ? std::shared_ptr<Event>() : events[eventIndex];
    const auto hardware = mpc.getHardware();

    if (auto note = std::dynamic_pointer_cast<NoteOnEvent>(event))
    {
        note->setTrack(trackIndex);

        if (sequencer->isOverdubbing() &&
            mpc.getHardware()->getButton(hardware::ComponentId::ERASE)->isPressed() &&
            (isActiveTrackIndex || recordingModeIsMulti) &&
            trackIndex < 64 &&
            busNumber > 0)
        {
            const auto sampler = mpc.getSampler();

            const std::shared_ptr<mpc::sampler::Program> program =
                sampler->getProgram(sampler->getDrumBusProgramIndex(busNumber));

            const auto noteNumber = note->getNote();

            bool oneOrMorePadsArePressed = false;
            bool noteIsPressed = false;

            for (auto &p : hardware->getPads())
            {
                if (p->isPressed())
                {
                    oneOrMorePadsArePressed = true;

                    // The pad index should be with bank at the time the pad was pressed
                    if (program->getNoteFromPad(p->getIndex()) == noteNumber)
                    {
                        noteIsPressed = true;
                        break;
                    }
                }
            }

            if (!_delete && oneOrMorePadsArePressed && mpc.isSixteenLevelsEnabled())
            {
                auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>();
                auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>();

                if (vmpcSettingsScreen->_16LevelsEraseMode == 0)
                {
                    _delete = true;
                }
                else if (vmpcSettingsScreen->_16LevelsEraseMode == 1)
                {
                    const auto &varValue = note->getVariationValue();
                    auto _16l_key = assign16LevelsScreen->getOriginalKeyPad();
                    auto _16l_type = assign16LevelsScreen->getType();

                    for (auto &pad : hardware->getPads())
                    {
                        if (!pad->isPressed())
                        {
                            continue;
                        }

                        int wouldBeVarValue;
                        auto padIndexWithoutBank = pad->getIndex();

                        if (_16l_type == 0)
                        {
                            auto diff = padIndexWithoutBank - _16l_key;
                            auto candidate = 64 + (diff * 5);

                            if (candidate > 124)
                            {
                                candidate = 124;
                            }
                            else if (candidate < 4)
                            {
                                candidate = 4;
                            }

                            wouldBeVarValue = static_cast<int>(candidate);
                        }
                        else
                        {
                            wouldBeVarValue = static_cast<int>(floor(100 / 16.0) * padIndexWithoutBank);
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

    if (isOn() && (!sequencer->isSoloEnabled() || sequencer->getActiveTrackIndex() == trackIndex))
    {
        mpc.getEventHandler()->handleFinalizedEvent(event, this);
    }

    eventIndex++;
}

bool Track::isOn()
{
    return on;
}

bool Track::isUsed()
{
    return used || !events.empty();
}

std::vector<std::shared_ptr<Event>> Track::getEventRange(int startTick, int endTick)
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

void Track::correctTimeRange(int startPos, int endPos, int stepLength, int swingPercentage, int lowestNote, int highestNote)
{
    auto s = mpc.getSequencer()->getActiveSequence();
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

            if (noteEvent->getTick() >= startPos && noteEvent->getTick() < endPos &&
                noteEvent->getNote() >= lowestNote && noteEvent->getNote() <= highestNote)
            {
                timingCorrect(fromBar, toBar, noteEvent, stepLength, swingPercentage);
            }
        }
    }

    removeDoubles();
}

void Track::timingCorrect(int fromBar, int toBar, const std::shared_ptr<NoteOnEvent> &noteEvent, int stepLength, int swingPercentage)
{
    auto event = std::dynamic_pointer_cast<Event>(noteEvent);
    updateEventTick(event, timingCorrectTick(fromBar, toBar, noteEvent->getTick(), stepLength, swingPercentage));
}

int Track::timingCorrectTick(int fromBar, int toBar, int tick, int stepLength, int swingPercentage)
{
    int accumBarLengths = 0;
    int previousAccumBarLengths = 0;
    auto barNumber = 0;
    auto numberOfSteps = 0;
    auto sequence = mpc.getSequencer()->getActiveSequence();
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
        if (sequence->getBarLengthsInTicks()[barNumber] - (i * stepLength) < 0)
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
        int stepStart = ((i - 1) * stepLength) + (stepLength / 2);
        int stepEnd = (i * stepLength) + (stepLength / 2);

        if (tick - currentBarStart >= stepStart && tick - currentBarStart <= stepEnd)
        {
            tick = (i * stepLength) + currentBarStart;

            if (tick >= segmentEnd)
            {
                tick = segmentStart;
            }

            break;
        }
    }

    if ((stepLength == 24 || stepLength == 48) && (tick + stepLength) % (stepLength * 2) == 0 && swingPercentage > 50)
    {
        const int swingOffset = ((swingPercentage - 50.f) / 25.f) * (stepLength / 2);
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

            for (auto &n : notesAtTick)
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

    for (auto &i : deleteIndexList)
    {
        events.erase(events.begin() + i);
    }
}

template <typename t>
void moveEvent(std::vector<t> &v, size_t oldIndex, size_t newIndex)
{
    if (oldIndex > newIndex)
    {
        std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
    }
    else
    {
        std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
    }
}

void Track::updateEventTick(std::shared_ptr<Event> &e, int newTick)
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

std::vector<std::shared_ptr<NoteOnEvent>> Track::getNoteEvents()
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

void Track::shiftTiming(std::shared_ptr<Event> event, bool later, int amount, int lastTick)
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

bool Track::insertEventWhileRetainingSort(const std::shared_ptr<Event> &event, bool allowMultipleNoteEventsWithSameNoteOnSameTick)
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
                e2 && e2->getTick() == tick && e2->getNote() == noteEvent->getNote())
            {
                e2.swap(noteEvent);
                return false;
            }
        }
    }

    const bool insertRequired = !events.empty() && events.back()->getTick() >= tick;

    if (insertRequired)
    {
        auto insertAt = std::find_if(events.begin(),
                                     events.end(),
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
