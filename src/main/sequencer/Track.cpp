#include <sequencer/Track.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <audiomidi/EventHandler.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/MidiClockEvent.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/MixerEvent.hpp>

using namespace mpc::sequencer;
using namespace std;

Track::Track(mpc::sequencer::Sequence* parent, mpc::Mpc* mpc, int i) 
{
	this->parent = parent;
	this->mpc = mpc;
	sequencer = mpc->getSequencer();
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

std::weak_ptr<NoteEvent> Track::getNoteEvent(int tick, int note) {
	auto ev = getNoteEventsAtTick(tick);
	for (auto& e : ev) {
		if (e.lock()->getNote() == note)
			return e;
	}
	return weak_ptr<NoteEvent>();
}

void Track::setTrackIndex(int i)
{
    trackIndex = i;
}

int Track::getTrackIndex()
{
    return trackIndex;
}

weak_ptr<NoteEvent> Track::recordNoteOn()
{
	auto lSequencer = sequencer.lock();
	auto n = make_shared<NoteEvent>();
	
	n->setTick(lSequencer->getTickPosition());
	
	for (auto& noteOnEvent : queuedNoteOnEvents) {
		if (noteOnEvent->getNote() == n->getNote()) {
			auto noteOffEvent = new NoteEvent();
			noteOffEvent->setNote(n->getNote());
			noteOffEvent->setVelocity(0);
			noteOffEvent->setTick(lSequencer->getTickPosition());
			recordNoteOff(noteOffEvent);
			break;
		}
	}

	if (n->getTick() >= lSequencer->getCurrentlyPlayingSequence().lock()->getLastTick()) {
		n->setTick(0);
	}
	
	queuedNoteOnEvents.push_back(n);
	
	return n;
}

void Track::flushNoteCache()
{
	queuedNoteOnEvents.clear();
}

void Track::recordNoteOff(NoteEvent* n)
{
	auto note = n->getNote();
	shared_ptr<NoteEvent> noteOn;
	int eraseIndex = -1;
	for (auto& noteEvent : queuedNoteOnEvents) {
		eraseIndex++;
		if (noteEvent->getNote() == note) {
			noteOn.swap(noteEvent);
			break;
		}
	}

	if (!noteOn) {
		return;
	}

	if (eraseIndex != -1 && eraseIndex != queuedNoteOnEvents.size()) queuedNoteOnEvents.erase(queuedNoteOnEvents.begin() + eraseIndex);
	
	if (n->getTick() >= noteOn->getTick()) {
		int candidate = n->getTick() - noteOn->getTick();
		if (candidate < 1) candidate = 1;
		noteOn->setDuration(candidate);
	}
	else {
		noteOn->setDuration(sequencer.lock()->getLoopEnd() - 1 - noteOn->getTick());
	}
	addEventRealTime(noteOn);
	eventIndex++;
}

void Track::setUsed(bool b)
{
    used = b;
    if (used) {
        setChanged();
        notifyObservers(string("tracknumbername"));
    }
}

void Track::setOn(bool b)
{
    on = b;
    setChanged();
    notifyObservers(string("trackon"));
}

void Track::addEventRealTime(shared_ptr<Event> event)
{
    if (events.size() == 0) setUsed(true);

	for (auto& temp : events) {
		if (temp->getTick() == event->getTick()) {
			if (typeid(temp) == typeid(event)) {
				if (dynamic_pointer_cast<NoteEvent>(temp)->getNote() == dynamic_pointer_cast<NoteEvent>(event)->getNote()) {
					dynamic_pointer_cast<NoteEvent>(temp)->setDuration(dynamic_pointer_cast<NoteEvent>(event)->getDuration());
					dynamic_pointer_cast<NoteEvent>(temp)->setVelocity(dynamic_pointer_cast<NoteEvent>(event)->getVelocity());
					return;
				}
			}
		}
	}
    tcValue = mpc->getUis().lock()->getSequencerWindowGui()->getNoteValue();
	auto lSequencer = sequencer.lock();
	if (tcValue > 0 && dynamic_pointer_cast<NoteEvent>(event)) {
		timingCorrect(0, parent->getLastBar(), dynamic_cast<NoteEvent*>(event.get()), lSequencer->getTickValues()[tcValue]);
	}
	events.push_back(std::move(event));
    sortEvents();
}

void Track::removeEvent(weak_ptr<Event> event) {
	for (int i = 0; i < events.size(); i++) {
		if (events[i] == event.lock()) {
			events.erase(events.begin() + i);
		}
	}
	setChanged();
	notifyObservers(string("resetstepeditor"));
}

bool Track::adjustDurLastEvent(int newDur)
{
	auto lLastAdded = lastAdded.lock();
	if (!lLastAdded) return false;
	lLastAdded->setDuration(newDur);
	lastAdded.reset();
	setChanged();
	notifyObservers(string("resetstepeditor"));
	return true;
}

weak_ptr<NoteEvent> Track::addNoteEvent(int tick, int note) {
	auto candidate = getNoteEvent(tick, note);
	if (candidate.lock()) {
		candidate.lock()->setDuration(1);
		lastAdded = candidate;
		return candidate;
	}
	auto res = dynamic_pointer_cast<NoteEvent>(addEvent(tick, "note").lock());
	res->setNote(note);
	return res;
}

weak_ptr<Event> Track::addEvent(int tick, string type) {
	shared_ptr<Event> res;
	if (type.compare("note") == 0) {
		res = make_shared<NoteEvent>();
		lastAdded = dynamic_pointer_cast<NoteEvent>(res);
	}
	else if (type.compare("tempochange") == 0) {
		res = make_shared<TempoChangeEvent>(parent);
	}
	else if (type.compare("pitchbend") == 0) {
		res = make_shared<PitchBendEvent>();
	}
	else if (type.compare("controlchange") == 0) {
		res = make_shared<ControlChangeEvent>();
	}
	else if (type.compare("programchange") == 0) {
		res = make_shared<ProgramChangeEvent>();
	}
	else if (type.compare("channelpressure") == 0) {
		res = make_shared<ChannelPressureEvent>();
	}
	else if (type.compare("polypressure") == 0) {
		res = make_shared<PolyPressureEvent>();
	}
	else if (type.compare("systemexclusive") == 0) {
		res = make_shared<SystemExclusiveEvent>();
	}
	else if (type.compare("mixer") == 0) {
		res = make_shared<MixerEvent>();
	}
	if (events.size() == 0) setUsed(true);
	res->setTick(tick);
	events.push_back(res);
	sortEvents();
	setChanged();
	notifyObservers(string("resetstepeditor"));
	setChanged();
	notifyObservers(string(type) + "added");
	return res;
}

weak_ptr<Event> Track::cloneEvent(weak_ptr<Event> src) {

	auto seq = sequencer.lock()->getActiveSequence().lock().get();

	shared_ptr<Event> res;
	auto tce = dynamic_pointer_cast<TempoChangeEvent>(src.lock());
	auto mce = dynamic_pointer_cast<MidiClockEvent>(src.lock());
	auto ne = dynamic_pointer_cast<NoteEvent>(src.lock());
	auto me = dynamic_pointer_cast<MixerEvent>(src.lock());

	if (ne) {
		res = make_shared<NoteEvent>();
		ne->CopyValuesTo(res);
		//lastAdded = dynamic_pointer_cast<NoteEvent>(res);
	}
	else if (tce) {
		res = make_shared<TempoChangeEvent>(seq);
		tce->CopyValuesTo(res);
	}
	else if (mce) {
		res = make_shared<MidiClockEvent>(0);
		mce->CopyValuesTo(res);
	}
	else if (me) {
		res = make_shared<MixerEvent>();
		me->CopyValuesTo(res);
	}

	if (!used) setUsed(true);

	events.push_back(res);

	sortEvents();
	setChanged();
	notifyObservers(string("resetstepeditor"));
	return events.back();
}

void Track::removeEvent(int i)
{
    events.erase(events.begin() + i);
	setChanged();
	notifyObservers(string("resetstepeditor"));
}

void Track::removeEvents() {
	events.clear();
}

void Track::setVelocityRatio(int i)
{
	if (i < 1) {
		i = 1;
	}
	else if (i > 200) {
		i = 200;
	}
    velocityRatio = i;
    setChanged();
    notifyObservers(string("velocityratio"));
}

int Track::getVelocityRatio()
{
    return velocityRatio;
}

void Track::setProgramChange(int i)
{
    if (i < 0 || i > 128) return;
    programChange = i;
    setChanged();
    notifyObservers(string("programchange"));
}

int Track::getProgramChange()
{
    return programChange;
}

void Track::setBusNumber(int i)
{
    if (i < 0 || i > 4) return;
    busNumber = i;
    setChanged();
    notifyObservers(string("tracktype"));
}

int Track::getBusNumber()
{
    return busNumber;
}

void Track::setDeviceNumber(int i)
{
    if (i < 0 || i > 32) return;
    device = i;
    setChanged();
    notifyObservers(string("device"));
    setChanged();
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
    setChanged();
    notifyObservers(string("tracknumbername"));
}

string Track::getName()
{
	if (!used) return "(Unused)";
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
	if (eventIndex + 1 > events.size() && noteOffs.size() == 0) {
		return MAX_TICK;
	}
	
	eventAvailable = eventIndex < events.size();
	sort(noteOffs.begin(), noteOffs.end(), tickCmp);
	
	if (noteOffs.size() != 0) {
		for (auto& no : noteOffs) {
			if (eventAvailable) {
				if (no->getTick() < events[eventIndex]->getTick()) {
					return no->getTick();
				}
			}
			else {
				return no->getTick();
			}
		}
	}

	if (eventAvailable) {
		return events[eventIndex]->getTick();
	}
	
	return MAX_TICK;
}

void Track::playNext()
{
	if (eventIndex + 1 > events.size() && noteOffs.size() == 0) {
		return;
	}
	
	auto lSequencer = sequencer.lock();
	multi = lSequencer->isRecordingModeMulti();
	delete_ = lSequencer->isRecording() && (trackIndex == lSequencer->getActiveTrackIndex() || multi) && (trackIndex < 64);

	if (lSequencer->isOverDubbing() && mpc->getControls().lock()->isErasePressed() && (trackIndex == lSequencer->getActiveTrackIndex() || multi) && trackIndex < 64) {
		delete_ = true;
	}

	int counter = 0;
	sort(noteOffs.begin(), noteOffs.end(), tickCmp);
	
	for (auto& no : noteOffs) {	
		if (eventIndex + 1 > events.size() || no->getTick() < events[eventIndex]->getTick()) {
			if (!delete_) {
				mpc->getEventHandler().lock()->handle(no, this);
			}
			noteOffs.erase(noteOffs.begin() + counter);
			return;
		}
		counter++;
	}

	event = events[eventIndex];
	auto lEvent = event.lock();
	auto note = dynamic_pointer_cast<NoteEvent>(lEvent);
	if (note) {
		note->setTrack(trackIndex);
	}
	if (delete_) {
		events.erase(events.begin() + eventIndex);
		return;
	}

	mpc->getEventHandler().lock()->handle(lEvent, this);

	auto ne = dynamic_pointer_cast<NoteEvent>(lEvent);
	if (ne) {
		if (ne->getVelocity() > 0 && ne->getDuration() >= 0) {
			auto noteOff = ne->getNoteOff().lock();
			noteOff->setDuration(0);
			noteOff->setNote(ne->getNote());
			noteOff->setTrack(ne->getTrack());
			auto dur = ne->getDuration();
			if (dur < 1) dur = 1;

			noteOff->setTick(ne->getTick() + dur);
			noteOff->setVelocity(0);
			noteOffs.push_back(noteOff);
		}
	}
	
	if (!(ne && ne->getVelocity() == 0)) {
		eventIndex++;
	}
}

bool Track::tickCmp(weak_ptr<Event> a, weak_ptr<Event> b) {
	return a.lock()->getTick() < b.lock()->getTick();
}

bool Track::noteCmp(weak_ptr<NoteEvent> a, weak_ptr<NoteEvent> b) {
	return a.lock()->getNote() < b.lock()->getNote();
}

void Track::off(bool stop)
{
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
    if(i < 0 || i + 1 > events.size()) return;
    eventIndex = i;
}

vector<weak_ptr<Event>> Track::getEventRange(int startTick, int endTick)
{
	if (tempEvents.size() > 0) tempEvents.clear();
	for (auto& e : events) {
		if (e->getTick() >= startTick && e->getTick() <= endTick)
			tempEvents.push_back(e);
	}
	return tempEvents;
}

void Track::correctTimeRange(int startPos, int endPos, int stepLength)
{
	if (sequencer.expired())
		sequencer = mpc->getSequencer();

	auto lSequencer = sequencer.lock();

	auto s = lSequencer->getActiveSequence().lock();
	int accumBarLengths = 0;
	auto fromBar = 0;
	auto toBar = 0;
	for (int i = 0; i < 999; i++) {
		accumBarLengths += (*s->getBarLengths())[i];
		if (accumBarLengths >= startPos) {
			fromBar = i;
			break;
		}
	}
	for (int i = 0; i < 999; i++) {
		accumBarLengths += (*s->getBarLengths())[i];
		if (accumBarLengths > endPos) {
			toBar = i;
			break;
		}
	}

	for (auto& event : events) {
		auto ne = dynamic_pointer_cast<NoteEvent>(event);
		if (ne) {
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
	auto s = sequencer.lock()->getActiveSequence().lock();
	int segmentStart = 0;
	int segmentEnd = 0;
	for (int i = 0; i < 999; i++) {
		if (i < fromBar)
			segmentStart += (*s->getBarLengths())[i];

		if (i <= toBar) {
			segmentEnd += (*s->getBarLengths())[i];
		}
		else {
			break;
		}
	}
	for (int i = 0; i < 999; i++) {
		accumBarLengths += (*s->getBarLengths())[i];
		if (tick < accumBarLengths && tick >= previousAccumBarLengths) {
			barNumber = i;
			break;
		}
		previousAccumBarLengths = accumBarLengths;
	}
	for (int i = 1; i < 1000; i++) {
		if ((*s->getBarLengths())[barNumber] - (i * stepLength) < 0) {
			numberOfSteps = i - 1;
			break;
		}
	}
	int currentBarStart = 0;
	for (int i = 0; i < barNumber; i++) {
		currentBarStart += (*s->getBarLengths())[i];
	}

	for (int i = 0; i <= numberOfSteps; i++) {
		int stepStart = ((i - 1) * stepLength) + (stepLength / 2);
		int stepEnd = (i * stepLength) + (stepLength / 2);
		if (tick - currentBarStart >= stepStart && tick - currentBarStart <= stepEnd) {
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
	for (auto& e : events) {
		auto ne = dynamic_pointer_cast<NoteEvent>(e);
		if (ne) {
			if (lastTick != e->getTick()) {
				notesAtTick.clear();
			}

			bool contains = false;
			for (auto& n : notesAtTick) {
				if (n == ne->getNote()) {
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
	std::reverse(deleteIndexList.begin(), deleteIndexList.end());
	for (auto& i : deleteIndexList) {
		events.erase(events.begin() + i);
	}
}

void Track::sortEvents()
{
	stable_sort(events.begin(), events.end(), tickCmp);
}

vector<weak_ptr<NoteEvent>> Track::getNoteEvents()
{
	vector<weak_ptr<NoteEvent>> noteEvents;
	for (auto& e : events) {
		auto ne = dynamic_pointer_cast<NoteEvent>(e);
		if (ne) noteEvents.push_back(ne);
	}
	return noteEvents;
}

vector<weak_ptr<NoteEvent>> Track::getNoteEventsAtTick(int tick)
{
	vector<weak_ptr<NoteEvent>> noteEvents;
	for (auto& ne : getNoteEvents()) {
		if (ne.lock()->getTick() == tick) {
			noteEvents.push_back(ne);
		}
	}
	return noteEvents;
}

void Track::sortEventsByNotePerTick()
{
	for (auto& ne : getNoteEvents()) {
		sortEventsOfTickByNote(getNoteEventsAtTick(ne.lock()->getTick()));
	}
}

void Track::sortEventsOfTickByNote(vector<weak_ptr<NoteEvent>> noteEvents)
{
    sort(noteEvents.begin(), noteEvents.end(), noteCmp);
}

void Track::swing(int noteValue, int percentage, vector<int> noteRange)
{
	swing(getEvents(), noteValue, percentage, noteRange);
}

void Track::swing(vector<weak_ptr<Event>> eventsToSwing, int noteValue, int percentage, vector<int> noteRange)
{
	if (noteValue != 1 && noteValue != 3) return;

	for (auto& e : eventsToSwing) {
		auto ne = dynamic_pointer_cast<NoteEvent>(e.lock());
		if (ne) {
			if (ne->getNote() >= noteRange[0] && ne->getNote() <= noteRange[1]) {
				ne->setTick(swingTick(ne->getTick(), noteValue, percentage));
			}
		}
	}
}

int Track::swingTick(int tick, int noteValue, int percentage) {
	if (noteValue != 1 && noteValue != 3) return tick;
	auto base = 48;
	if (noteValue == 3) base = 24;
	if ((tick + base) % (base * 2) == 0) {
		tick += ((percentage - 50) * (4.0 / 100.0) * (base / 2.0));
	}
	return tick;
}

void Track::shiftTiming(bool later, int amount, int lastTick)
{
	shiftTiming(getEvents(), later, amount, lastTick);
}

void Track::shiftTiming(vector<weak_ptr<Event>> eventsToShift, bool later, int amount, int lastTick)
{
	if (!later) amount *= -1;
	for (auto& event : eventsToShift) {
		auto e = event.lock();
		e->setTick(e->getTick() + amount);
		if (e->getTick() < 0) {
			e->setTick(0);
		}
		if (e->getTick() > lastTick) {
			e->setTick(lastTick);
		}
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

Track::~Track() {
}
