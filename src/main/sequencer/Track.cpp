#include <sequencer/Track.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>
#include <audiomidi/EventHandler.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/MidiClockEvent.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ChannelPressureEvent.hpp>

#include <lcdgui/screens/PunchScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/Assign16LevelsScreen.hpp>

#include <lcdgui/screens/VmpcSettingsScreen.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>
#include <hardware/TopPanel.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

using namespace mpc::sequencer;

Track::Track(mpc::Mpc& mpc, mpc::sequencer::Sequence* parent, int i)
	: mpc(mpc)
{
	this->parent = parent;

	sequencer = mpc.getSequencer();
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

	if (tick > oldTick) {
		if (eventIndex == events.size()) {
			return;
		}
		startIndex = eventIndex;
	}

	if (tick < oldTick && eventIndex == 0) {
		return;
	}

	eventIndex = events.size();

	for (int i = startIndex; i < events.size(); i++) {
		if (events[i]->getTick() >= tick) {
			eventIndex = i;
			break;
		}
	}
}

std::shared_ptr<NoteEvent> Track::getNoteEvent(int tick, int note) {
	auto ev = getNoteEventsAtTick(tick);
	for (auto& e : ev)
    {
		if (e->getNote() == note)
			return e;
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

// This is called from the UI thread. Results in incorrect tickpos.
// We should only queue the fact that a note of n wants to be recorded.
// Then we let getNextTick, from the audio thread, set the tickpos.
std::shared_ptr<NoteEvent> Track::recordNoteOnNow(unsigned char note)
{
	auto n = std::make_shared<NoteEvent>(note);
    n->setTick(-2);
	queuedNoteOnEvents.enqueue(n);
	return n;
}

void Track::flushNoteCache()
{
    std::shared_ptr<NoteEvent> e;
	while (queuedNoteOnEvents.try_dequeue(e)) {}
    while (queuedNoteOffEvents.try_dequeue(e)) {}
}

void Track::recordNoteOffNow(unsigned char note)
{
    auto event = std::make_shared<NoteEvent>(note);
    event->setTick(-2);
    queuedNoteOffEvents.enqueue(event);
}

void Track::setUsed(bool b)
{
	if (!used && b && trackIndex < 64)
		name = mpc.getSequencer()->getDefaultTrackName(trackIndex);

	used = b;

	if (used)
        notifyObservers(std::string("tracknumbername"));
}

void Track::setOn(bool b)
{
    on = b;

    notifyObservers(std::string("trackon"));
}

void Track::removeEvent(const std::shared_ptr<Event>& event)
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

bool Track::adjustDurLastEvent(int newDur)
{
	auto lLastAdded = lastAdded.lock();

	if (!lLastAdded)
		return false;

	lLastAdded->setDuration(newDur);
	lastAdded.reset();

	notifyObservers(std::string("adjust-duration"));
	return true;
}

std::shared_ptr<NoteEvent> Track::addNoteEvent(int tick, int note)
{
	auto candidate = getNoteEvent(tick, note);

	if (candidate)
	{
		candidate->setDuration(1);
		lastAdded = candidate;
		return candidate;
	}

	auto res = std::dynamic_pointer_cast<NoteEvent>(addEvent(tick, "note"));
	res->setNote(note);

	notifyObservers(std::string("step-editor"));

	return res;
}

std::shared_ptr<Event> Track::addEvent(int tick, const std::string& type, bool allowMultipleNotesOnSameTick)
{
	std::shared_ptr<Event> res;

	if (type == "note")
	{
		res = std::make_shared<NoteEvent>();
		lastAdded = std::dynamic_pointer_cast<NoteEvent>(res);
	}
	else if (type == "tempo-change")
	{
		res = std::make_shared<TempoChangeEvent>(parent);
	}
	else if (type == "pitchbend")
	{
		res = std::make_shared<PitchBendEvent>();
	}
	else if (type == "controlchange")
	{
		res = std::make_shared<ControlChangeEvent>();
	}
	else if (type == "programchange")
	{
		res = std::make_shared<ProgramChangeEvent>();
	}
	else if (type == "channelpressure")
	{
		res = std::make_shared<ChannelPressureEvent>();
	}
	else if (type == "polypressure")
	{
		res = std::make_shared<PolyPressureEvent>();
	}
	else if (type == "systemexclusive")
	{
		res = std::make_shared<SystemExclusiveEvent>();
	}
	else if (type == "mixer")
	{
		res = std::make_shared<MixerEvent>();
	}

	if (events.empty())
	{
		setUsed(true);
	}

	res->setTick(tick);

    insertEventWhileRetainingSort(res, allowMultipleNotesOnSameTick);

	notifyObservers(std::string("step-editor"));
	notifyObservers(std::string(type) + "added");

	return res;
}

void Track::cloneEventIntoTrack(std::shared_ptr<Event> &src, int tick, bool allowMultipleNotesOnSameTick)
{
    auto seq = sequencer->getActiveSequence().get();

    std::shared_ptr<Event> res;

    auto cpe = std::dynamic_pointer_cast<ChannelPressureEvent>(src);
    auto cce = std::dynamic_pointer_cast<ControlChangeEvent>(src);
    auto ppe = std::dynamic_pointer_cast<PolyPressureEvent>(src);
    auto pce = std::dynamic_pointer_cast<ProgramChangeEvent>(src);
    auto see = std::dynamic_pointer_cast<SystemExclusiveEvent>(src);
    auto tce = std::dynamic_pointer_cast<TempoChangeEvent>(src);
    auto mce = std::dynamic_pointer_cast<MidiClockEvent>(src);
    auto ne = std::dynamic_pointer_cast<NoteEvent>(src);
    auto me = std::dynamic_pointer_cast<MixerEvent>(src);
    auto pbe = std::dynamic_pointer_cast<PitchBendEvent>(src);

    if (ne)
    {
        res = std::make_shared<NoteEvent>();
        ne->CopyValuesTo(res);
    }
    else if (tce)
    {
        res = std::make_shared<TempoChangeEvent>(seq);
        tce->CopyValuesTo(res);
    }
    else if (mce)
    {
        res = std::make_shared<MidiClockEvent>(0);
        mce->CopyValuesTo(res);
    }
    else if (me)
    {
        res = std::make_shared<MixerEvent>();
        me->CopyValuesTo(res);
    }
    else if (pbe)
    {
        res = std::make_shared<PitchBendEvent>();
        pbe->CopyValuesTo(res);
    }
    else if (cpe)
    {
        res = std::make_shared<ChannelPressureEvent>();
        cpe->CopyValuesTo(res);
    }
    else if (cce)
    {
        res = std::make_shared<ControlChangeEvent>();
        cce->CopyValuesTo(res);
    }
    else if (ppe)
    {
        res = std::make_shared<PolyPressureEvent>();
        ppe->CopyValuesTo(res);
    }
    else if (pce)
    {
        res = std::make_shared<ProgramChangeEvent>();
        pce->CopyValuesTo(res);
    }
    else if (see)
    {
        res = std::make_shared<SystemExclusiveEvent>();
        see->CopyValuesTo(res);
    }

    if (!res)
    {
        return;
    }

    if (!used)
    {
        setUsed(true);
    }

    res->setTick(tick);

    insertEventWhileRetainingSort(res, allowMultipleNotesOnSameTick);
    notifyObservers(std::string("step-editor"));
}

void Track::removeEvent(int i)
{
    events.erase(events.begin() + i);

	notifyObservers(std::string("step-editor"));
}

void Track::removeEvents() {
	events.clear();
}

void Track::setVelocityRatio(int i)
{
	if (i < 1)
		i = 1;
	else if (i > 200)
		i = 200;

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
		return;

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
		return;

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
		return "(Unused)";

    return name;
}

std::vector<std::shared_ptr<Event>>& Track::getEvents()
{
    return events;
}

int Track::getCorrectedTickPos()
{
    auto pos = sequencer->getTickPosition();
    auto correctedTickPos = -1;

    auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
    auto tcValue = timingCorrectScreen->getNoteValue();
    auto swingPercentage = timingCorrectScreen->getSwing();

    if (tcValue > 0)
    {
        correctedTickPos = timingCorrectTick(0, parent->getLastBarIndex(), pos, sequencer->getTickValues()[tcValue], swingPercentage);
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

    auto pos = this->sequencer->getTickPosition();
    auto correctedTickPos = getCorrectedTickPos();

    for (int noteOffIndex = 0; noteOffIndex < noteOffCount; noteOffIndex++) {
        auto noteOff = bulkNoteOffs[noteOffIndex];
        if (noteOff->getTick() == -2) {
            noteOff->setTick(pos);
        }
    }

    for (int noteOnIndex = 0; noteOnIndex < noteOnCount; noteOnIndex++) {

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
                if (newTick < 0) newTick += parent->getLastTick();
                else if (newTick > parent->getLastTick()) newTick -= parent->getLastTick();
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

                if (fixEventIndex && wasInserted) eventIndex--;

                needsToBeRequeued = false;
            } else {
                this->queuedNoteOffEvents.enqueue(noteOff);
            }
        }

        if (needsToBeRequeued) {
            this->queuedNoteOnEvents.enqueue(noteOn);
        }
    }
}

int Track::getNextTick()
{
	if (eventIndex >= events.size()) {
        processRealtimeQueuedEvents();
        return MAX_TICK;
    }

	processRealtimeQueuedEvents();
    return events[eventIndex]->getTick();
}

void Track::playNext()
{
	if (eventIndex >= events.size())
		return;

	multi = sequencer->isRecordingModeMulti();
	_delete = sequencer->isRecording() && (trackIndex == sequencer->getActiveTrackIndex() || multi) && (trackIndex < 64);

	auto punchScreen = mpc.screens->get<PunchScreen>("punch");

	if (sequencer->isRecording() && punchScreen->on && trackIndex < 64)
	{
		auto pos = sequencer->getTickPosition();

		_delete = false;

		if (punchScreen->autoPunch == 0 && pos >= punchScreen->time0)
			_delete = true;

		if (punchScreen->autoPunch == 1 && pos < punchScreen->time1)
			_delete = true;

		if (punchScreen->autoPunch == 2 && pos >= punchScreen->time0 && pos < punchScreen->time1)
			_delete = true;
	}

    auto event = eventIndex >= events.size() ? std::shared_ptr<Event>() : events[eventIndex];
	auto note = std::dynamic_pointer_cast<NoteEvent>(event);

	if (note)
    {
        note->setTrack(trackIndex);

        if (sequencer->isOverDubbing() &&
            mpc.getControls()->isErasePressed() &&
            (trackIndex == sequencer->getActiveTrackIndex() || multi) &&
            trackIndex < 64 &&
            busNumber > 0)
        {
            auto pgmIndex = mpc.getSampler()->getDrumBusProgramIndex(busNumber);
            auto pgm = mpc.getSampler()->getProgram(pgmIndex);

            bool oneOrMorePadsArePressed = false;
            auto hardware = mpc.getHardware();

            for (auto& p : hardware->getPads())
            {
              if (p->isPressed())
              {
                oneOrMorePadsArePressed = true;
                break;
              }
            }

            if (!_delete && oneOrMorePadsArePressed && hardware->getTopPanel()->isSixteenLevelsEnabled())
            {
                auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
                auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>("assign-16-levels");

                if (vmpcSettingsScreen->_16LevelsEraseMode == 0)
                {
                    _delete = true;
                }
                else if (vmpcSettingsScreen->_16LevelsEraseMode == 1)
                {
                    const auto& varValue = note->getVariationValue();
                    std::vector<int> pressedNotes;
                    auto _16l_key = assign16LevelsScreen->getOriginalKeyPad();
                    auto _16l_type = assign16LevelsScreen->getType();

                    for (auto& hwPad : hardware->getPads())
                    {
                        if (hwPad->isPressed())
                        {
                            int wouldBeVarValue;
                            auto padIndexWithoutBank = hwPad->getPadIndexWithBankWhenLastPressed() % 16;

                            if (_16l_type == 0)
                            {
                                auto diff = padIndexWithoutBank - _16l_key;
                                auto candidate = 64 + (diff * 5);

                                if (candidate > 124)
                                    candidate = 124;
                                else if (candidate < 4)
                                    candidate = 4;

                                wouldBeVarValue = static_cast<int>(candidate);
                            }
                            else
                            {
                                wouldBeVarValue = static_cast<int>(floor(100 / 16.0) * padIndexWithoutBank);
                            }

                            if (varValue == wouldBeVarValue)
                                _delete = true;
                        }
                    }
                }
            }
        }
    }

	if (_delete && !events[eventIndex]->dontDelete)
	{
        events.erase(events.begin() + eventIndex);
		return;
	}

    events[eventIndex]->dontDelete = false;

	mpc.getEventHandler()->handle(event, this);
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

	for (auto& e : events)
	{
		if (e->getTick() >= startTick && e->getTick() <= endTick)
			res.push_back(e);
	}

	return res;
}

void Track::correctTimeRange(int startPos, int endPos, int stepLength, int swingPercentage, int lowestNote, int highestNote)
{
	auto s = sequencer->getActiveSequence();
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

	for (auto& event : events)
	{
		if (auto ne = std::dynamic_pointer_cast<NoteEvent>(event))
		{
			if (event->getTick() >= endPos)
				break;

			if (event->getTick() >= startPos && event->getTick() < endPos)
            {
                if (ne->getNote() >= lowestNote && ne->getNote() <= highestNote)
                timingCorrect(fromBar, toBar, ne, stepLength, swingPercentage);
            }
		}
	}

	removeDoubles();
}

void Track::timingCorrect(int fromBar, int toBar, std::shared_ptr<NoteEvent> noteEvent, int stepLength, int swingPercentage)
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
	auto sequence = sequencer->getActiveSequence();
	int segmentStart = 0;
	int segmentEnd = 0;

	for (int i = 0; i < 999; i++)
	{
		if (i < fromBar)
			segmentStart += sequence->getBarLengthsInTicks()[i];

		if (i <= toBar) {
			segmentEnd += sequence->getBarLengthsInTicks()[i];
		}
		else {
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
		currentBarStart += sequence->getBarLengthsInTicks()[i];

	for (int i = 0; i <= numberOfSteps; i++)
	{
		int stepStart = ((i - 1) * stepLength) + (stepLength / 2);
		int stepEnd = (i * stepLength) + (stepLength / 2);

		if (tick - currentBarStart >= stepStart && tick - currentBarStart <= stepEnd)
		{
			tick = (i * stepLength) + currentBarStart;

			if (tick >= segmentEnd)
				tick = segmentStart;

			break;
		}
	}

    if (swingPercentage > 50)
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

	for (auto& e : events)
	{
		auto ne = std::dynamic_pointer_cast<NoteEvent>(e);

		if (ne)
		{
			if (lastTick != e->getTick())
				notesAtTick.clear();

			bool contains = false;

			for (auto& n : notesAtTick)
			{
				if (n == ne->getNote())
				{
					contains = true;
					break;
				}
			}

			if (!contains) {
				notesAtTick.push_back(ne->getNote());
			}
			else {
				deleteIndexList.push_back(eventCounter);
			}

			lastTick = e->getTick();
		}
		eventCounter++;
	}

	reverse(deleteIndexList.begin(), deleteIndexList.end());

	for (auto& i : deleteIndexList)
		events.erase(events.begin() + i);
}

template <typename t> void moveEvent(std::vector<t>& v, size_t oldIndex, size_t newIndex)
{
    if (oldIndex > newIndex)
        std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
    else
        std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

void Track::updateEventTick(std::shared_ptr<Event>& e, int newTick)
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

std::vector<std::shared_ptr<NoteEvent>> Track::getNoteEvents()
{
	std::vector<std::shared_ptr<NoteEvent>> noteEvents;

	for (auto& e : events)
	{
		auto ne = std::dynamic_pointer_cast<NoteEvent>(e);

		if (ne)
			noteEvents.push_back(ne);
	}

	return noteEvents;
}

std::vector<std::shared_ptr<NoteEvent>> Track::getNoteEventsAtTick(int tick)
{
	std::vector<std::shared_ptr<NoteEvent>> noteEvents;

	for (auto& ne : getNoteEvents())
	{
		if (ne->getTick() == tick)
			noteEvents.push_back(ne);
	}

	return noteEvents;
}

void Track::shiftTiming(std::shared_ptr<Event> event, bool later, int amount, int lastTick)
{
    if (!later)
        amount *= -1;

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

bool Track::insertEventWhileRetainingSort(const std::shared_ptr<Event>& event, bool allowMultipleNotesOnSameTick)
{
    if (!isUsed())
    {
        setUsed(true);
    }

    auto tick = event->getTick();

    auto noteEvent = std::dynamic_pointer_cast<NoteEvent>(event);

    if (noteEvent && !allowMultipleNotesOnSameTick) {
        for (auto &e: getNoteEventsAtTick(tick)) {
            if (e->getNote() == noteEvent->getNote()) {
                e.swap(noteEvent);
                return false;
            }
        }
    }

    const bool insertRequired = !events.empty() && events.back()->getTick() >= tick;

    if (insertRequired)
    {
        auto insertAt = std::find_if(events.begin(),
                                     events.end(),
                                     [&tick](const std::shared_ptr<Event>& e) { return e->getTick() > tick; });

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
