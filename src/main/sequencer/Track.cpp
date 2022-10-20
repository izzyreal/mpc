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
using namespace std;

Track::Track(mpc::Mpc& mpc, mpc::sequencer::Sequence* parent, int i)
	: mpc(mpc)
{
	this->parent = parent;

	sequencer = mpc.getSequencer().lock();
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
	if (tick == 0) {
		eventIndex = 0;
		return;
	}

	sort(noteOffs.begin(), noteOffs.end(), tickCmp);

	for (auto& no : noteOffs) {
		no->setTick((no->getTick() - oldTick) + tick);
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

weak_ptr<NoteEvent> Track::recordNoteOn()
{
	auto punchScreen = mpc.screens->get<PunchScreen>("punch");

	auto pos = sequencer->getTickPosition();

	if (sequencer->isRecordingOrOverdubbing() && punchScreen->on)
	{
		auto mode = punchScreen->autoPunch;

		if (mode == 0 && pos < punchScreen->time0)
			return {};

		if (mode == 1 && pos >= punchScreen->time1)
			return {};

		if (mode == 2 && (pos < punchScreen->time0 || pos >= punchScreen-> time1))
			return {};
	}

	auto n = std::make_shared<NoteEvent>();

	n->setTick(pos);

	for (auto& noteOnEvent : queuedNoteOnEvents)
	{
		if (noteOnEvent->getNote() == n->getNote())
		{
			NoteEvent noteOff;
			noteOff.setNote(n->getNote());
			noteOff.setVelocity(0);
			noteOff.setTick(pos);
			recordNoteOff(noteOff);
			break;
		}
	}

	if (n->getTick() >= sequencer->getCurrentlyPlayingSequence().lock()->getLastTick())
        n->setTick(0);

	queuedNoteOnEvents.push_back(n);

	return n;
}

void Track::flushNoteCache()
{
	queuedNoteOnEvents.clear();
}

void Track::recordNoteOff(NoteEvent& n)
{
    auto note = n.getNote();
    shared_ptr<NoteEvent> noteOn;
    int eraseIndex = -1;

    for (auto& noteEvent : queuedNoteOnEvents)
    {
        eraseIndex++;

        if (noteEvent->getNote() == note)
        {
            noteOn.swap(noteEvent);
            break;
        }
    }

    if (!noteOn)
        return;

    if (eraseIndex != -1 && eraseIndex != queuedNoteOnEvents.size())
        queuedNoteOnEvents.erase(queuedNoteOnEvents.begin() + eraseIndex);

    if (n.getTick() >= noteOn->getTick())
    {
        int candidate = n.getTick() - noteOn->getTick();

        if (candidate < 1)
            candidate = 1;

        noteOn->setDuration(candidate);
    }
    else
    {
        noteOn->setDuration(sequencer->getLoopEnd() - 1 - noteOn->getTick());
    }

    auto eventsSize = events.size();

    addEventRealTime(noteOn);

    if (eventsSize != events.size())
    {
        eventIndex++;
    }
}

void Track::setUsed(bool b)
{
	if (!used && b && trackIndex < 64)
		name = mpc.getSequencer().lock()->getDefaultTrackName(trackIndex);

	used = b;

	if (used)
        notifyObservers(string("tracknumbername"));
}

void Track::setOn(bool b)
{
    on = b;

    notifyObservers(string("trackon"));
}

void Track::addEventRealTime(shared_ptr<NoteEvent> e1)
{
	auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
	auto tcValue = timingCorrectScreen->getNoteValue();

	if (tcValue > 0 && e1)
    {
        timingCorrect(0, parent->getLastBarIndex(), e1.get(), sequencer->getTickValues()[tcValue]);
        e1->setTick(swingTick(e1->getTick(), tcValue, timingCorrectScreen->getSwing()));
    }

    for (auto& _e2 : events)
    {
        if (_e2->getTick() == e1->getTick())
        {
            auto e2 = dynamic_pointer_cast<NoteEvent>(_e2);

            if (!e2) continue;

            if (e2->getNote() == e1->getNote())
            {
                e2->setDuration(e1->getDuration());
                e2->setVelocity(e1->getVelocity());
                return;
            }
        }
    }

    if (events.size() == 0)
        setUsed(true);

	events.push_back(std::move(e1));
    sortEvents();
}

void Track::removeEvent(weak_ptr<Event> event)
{
	for (int i = 0; i < events.size(); i++)
	{
		if (events[i] == event.lock())
			events.erase(events.begin() + i);
	}

	notifyObservers(string("step-editor"));
}

bool Track::adjustDurLastEvent(int newDur)
{
	auto lLastAdded = lastAdded.lock();

	if (!lLastAdded)
		return false;

	lLastAdded->setDuration(newDur);
	lastAdded.reset();

	notifyObservers(string("adjust-duration"));
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

	auto res = dynamic_pointer_cast<NoteEvent>(addEvent(tick, "note"));
	res->setNote(note);

	notifyObservers(string("step-editor"));

	return res;
}

std::shared_ptr<Event> Track::addEvent(int tick, const std::string& type)
{
    const bool sortRequired = !events.empty() && events.back()->getTick() >= tick;

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
	events.push_back(res);

    if (sortRequired)
    {
        sortEvents();
    }

	notifyObservers(string("step-editor"));

	notifyObservers(string(type) + "added");
	return res;
}

weak_ptr<Event> Track::cloneEvent(weak_ptr<Event> src)
{

	auto seq = sequencer->getActiveSequence().lock().get();

	shared_ptr<Event> res;
	auto tce = dynamic_pointer_cast<TempoChangeEvent>(src.lock());
	auto mce = dynamic_pointer_cast<MidiClockEvent>(src.lock());
	auto ne = dynamic_pointer_cast<NoteEvent>(src.lock());
	auto me = dynamic_pointer_cast<MixerEvent>(src.lock());

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

	if (!used)
		setUsed(true);

	auto result = weak_ptr<Event>(res);

	events.push_back(res);

	sortEvents();

	notifyObservers(string("step-editor"));
	return result;
}

void Track::removeEvent(int i)
{
    events.erase(events.begin() + i);

	notifyObservers(string("step-editor"));
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

    notifyObservers(string("velocityratio"));
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

    notifyObservers(string("programchange"));
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

    notifyObservers(string("bus"));
}

int Track::getBus()
{
    return busNumber;
}

void Track::setDeviceNumber(int i)
{
	if (i < 0 || i > 32)
		return;

    device = i;

    notifyObservers(string("device"));

    notifyObservers(string("devicename"));
}

int Track::getDevice()
{
    return device;
}

weak_ptr<Event> Track::getEvent(int i)
{
	return events[i];
}

void Track::setName(string s)
{
    name = s;

    notifyObservers(string("tracknumbername"));
}

string Track::getName()
{
	if (!used)
		return "(Unused)";

    return name;
}

vector<weak_ptr<Event>> Track::getEvents()
{
	auto res = vector<weak_ptr<Event>>();

	for (auto& e : events)
		res.push_back(e);

    return res;
}

int Track::getNextTick()
{
	if (eventIndex >= events.size() && noteOffs.empty())
		return MAX_TICK;

    sort(noteOffs.begin(), noteOffs.end(), tickCmp);

    auto noteOnAvailable = eventIndex < events.size();

    for (auto& noteOff : noteOffs)
    {
        if (noteOnAvailable)
        {
            if (noteOff->getTick() < events[eventIndex]->getTick())
                return noteOff->getTick();
        }
        else
        {
            return noteOff->getTick();
        }
    }

	if (eventIndex < events.size())
		return events[eventIndex]->getTick();

	return MAX_TICK;
}

void Track::playNext()
{
	if (eventIndex >= events.size() && noteOffs.empty())
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

	int counter = 0;
    sortEvents();

    auto event = eventIndex >= events.size() ? shared_ptr<Event>() : events[eventIndex];

	for (auto& no : noteOffs)
	{
		if (eventIndex >= events.size() || no->getTick() < event->getTick())
		{
			if (!_delete)
				mpc.getEventHandler().lock()->handle(no, this);

			noteOffs.erase(noteOffs.begin() + counter);
			return;
		}
		counter++;
	}

	auto note = dynamic_pointer_cast<NoteEvent>(event);

	if (note)
    {
        note->setTrack(trackIndex);

        if (sequencer->isOverDubbing() &&
            mpc.getControls().lock()->isErasePressed() &&
            (trackIndex == sequencer->getActiveTrackIndex() || multi) &&
            trackIndex < 64 &&
            busNumber > 0)
        {
            auto pgmIndex = mpc.getSampler().lock()->getDrumBusProgramNumber(busNumber);
            auto pgm = mpc.getSampler().lock()->getProgram(pgmIndex).lock();

            bool oneOrMorePadsArePressed = false;
            auto hardware = mpc.getHardware().lock();
            
          for (auto& p : hardware->getPads())
            {
              if (p->isPressed())
              {
                oneOrMorePadsArePressed = true;
                break;
              }
            }
          
            if (!_delete && oneOrMorePadsArePressed &&
                hardware->getTopPanel().lock()->isSixteenLevelsEnabled())
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
                    vector<int> pressedNotes;
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

	if (_delete)
	{
		events.erase(events.begin() + eventIndex);
		return;
	}

	mpc.getEventHandler().lock()->handle(event, this);

	if (note)
	{
		if (note->getVelocity() > 0 && note->getDuration() >= 0)
		{
			auto noteOff = note->getNoteOff().lock();
			noteOff->setDuration(0);
			noteOff->setNote(note->getNote());
			noteOff->setTrack(note->getTrack());
			auto dur = note->getDuration();

			if (dur < 1) dur = 1;

			noteOff->setTick(note->getTick() + dur);
			noteOff->setVelocity(0);
			noteOffs.push_back(noteOff);
		}
	}

	if (!(note && note->getVelocity() == 0))
		eventIndex++;
}

bool Track::tickCmp(const std::shared_ptr<Event>& a, const std::shared_ptr<Event>& b)
{
	return a->getTick() < b->getTick();
}

bool Track::isOn()
{
    return on;
}

bool Track::isUsed()
{
    return used || events.size() > 0;
}

void Track::setEventIndex(int i)
{
	if (i < 0 || i >= events.size())
		return;

    eventIndex = i;
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

void Track::correctTimeRange(int startPos, int endPos, int stepLength)
{
	auto s = sequencer->getActiveSequence().lock();
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
		auto ne = dynamic_pointer_cast<NoteEvent>(event);

		if (ne)
		{
			if (event->getTick() >= endPos)
				break;

			if (event->getTick() >= startPos && event->getTick() < endPos)
				timingCorrect(fromBar, toBar, ne.get(), stepLength);
		}
	}

	removeDoubles();
	sortEvents();
}

void Track::timingCorrect(int fromBar, int toBar, NoteEvent* noteEvent, int stepLength)
{
	auto newTick = timingCorrectTick(fromBar, toBar, noteEvent->getTick(), stepLength);

	if (newTick != noteEvent->getTick())
		noteEvent->setTick(newTick);
}

int Track::timingCorrectTick(int fromBar, int toBar, int tick, int stepLength)
{
	int accumBarLengths = 0;
	int previousAccumBarLengths = 0;
	auto barNumber = 0;
	auto numberOfSteps = 0;
	auto sequence = sequencer->getActiveSequence().lock();
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

	return tick;
}

void Track::removeDoubles()
{
	auto eventCounter = 0;
	vector<int> deleteIndexList;
	vector<int> notesAtTick;
	int lastTick = -100;

	for (auto& e : events)
	{
		auto ne = dynamic_pointer_cast<NoteEvent>(e);

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

void Track::sortEvents()
{
	sort(events.begin(), events.end(), tickCmp);
}

std::vector<std::shared_ptr<NoteEvent>> Track::getNoteEvents()
{
	std::vector<shared_ptr<NoteEvent>> noteEvents;

	for (auto& e : events)
	{
		auto ne = dynamic_pointer_cast<NoteEvent>(e);

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

void Track::swing(std::vector<std::shared_ptr<Event>>& eventsToSwing, int noteValue, int percentage, std::vector<int>& noteRange)
{
	if (noteValue != 1 && noteValue != 3)
		return;

	for (auto& e : eventsToSwing)
	{
		auto ne = dynamic_pointer_cast<NoteEvent>(e);

		if (ne)
		{
			if (ne->getNote() >= noteRange[0] && ne->getNote() <= noteRange[1])
				ne->setTick(swingTick(ne->getTick(), noteValue, percentage));
		}
	}
}

int Track::swingTick(int tick, int noteValue, int percentage)
{
	if (noteValue != 1 && noteValue != 3)
		return tick;

	auto base = 48;

	if (noteValue == 3)
		base = 24;

	if ((tick + base) % (base * 2) == 0)
		tick += ((percentage - 50) * (4.0 / 100.0) * (base / 2.0));

	return tick;
}

void Track::shiftTiming(std::vector<std::shared_ptr<Event>>& eventsToShift, bool later, int amount, int lastTick)
{
	if (!later)
		amount *= -1;

	for (auto& event : eventsToShift)
	{
		event->setTick(event->getTick() + amount);

		if (event->getTick() < 0)
			event->setTick(0);

		if (event->getTick() > lastTick)
			event->setTick(lastTick);
	}
}

int Track::getEventIndex()
{
    return eventIndex;
}

string Track::getActualName()
{
    return name;
}
