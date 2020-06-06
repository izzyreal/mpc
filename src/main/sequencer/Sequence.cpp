#include <sequencer/Sequence.hpp>

#include <Mpc.hpp>

#include <ui/UserDefaults.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/window/CountMetronomeScreen.hpp>
#include <lcdgui/screens/dialog/MetronomeSoundScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sequencer;
using namespace std;

Sequence::Sequence(vector<string> defaultTrackNames)
{
	this->defaultTrackNames = defaultTrackNames;
	barLengths = vector<int>(999);
	numerators = vector<int>(999);
	denominators = vector<int>(999);
	lastLoopBarEnd = true;

	tempoChangeOn = true;
	used = false;

	loopEnabled = true;
	lastBar = -1;
	for (int i = 0; i < 64; i++) {
		tracks.push_back(make_shared<Track>(this, i));
		tracks[i]->setName(defaultTrackNames[i]);
	}
	metaTracks.push_back(make_shared<Track>(this, 64));
	metaTracks.push_back(make_shared<Track>(this, 65));
	metaTracks.push_back(make_shared<Track>(this, 66));
	metaTracks[0]->setUsed(true);
	metaTracks[1]->setUsed(true);
	metaTracks[2]->setUsed(true);
	metaTracks[0]->setName("click");
	metaTracks[1]->setName("midiclock");
	metaTracks[2]->setName("tempo");
	deviceNames = vector<string>(33);

	auto& userDefaults = mpc::ui::UserDefaults::instance();

	for (int i = 0; i < 33; i++) {
		deviceNames[i] = userDefaults.getDeviceName(i);
	}
}

void Sequence::setLoopStart(int l)
{
	loopStart = l;
}

int Sequence::getLoopStart()
{
	return loopStart;
}

void Sequence::setLoopEnd(int l)
{
	loopEnd = l;
}

int Sequence::getLoopEnd()
{
	return loopEnd;
}

void Sequence::setFirstLoopBar(int i)
{
	if (i < 0 || i > lastBar) {
		return;
	}

	firstLoopBar = i;
	setChanged();
	notifyObservers(string("firstloopbar"));
	if (i > lastLoopBar) {
		lastLoopBar = i;
		setChanged();
		notifyObservers(string("lastloopbar"));
	}
}

int Sequence::getFirstLoopBar()
{
	return firstLoopBar;
}

void Sequence::setLastLoopBar(int i)
{
	if (i < 0) {
		return;
	}

	if (lastLoopBarEnd) {
		if (i < lastBar) {
			lastLoopBarEnd = false;
			lastLoopBar = lastBar;
			setChanged();
			notifyObservers(string("lastloopbar"));
			return;
		}
		else {
			return;
		}
	}
	else {
		if (i > lastBar) {
			lastLoopBarEnd = true;
			setChanged();
			notifyObservers(string("lastloopbar"));
		}
		else {
			lastLoopBar = i;
			setChanged();
			notifyObservers(string("lastloopbar"));
			if (i < firstLoopBar) {
				firstLoopBar = i;
				setChanged();
				notifyObservers(string("firstloopbar"));
			}
		}
	}
	lastLoopBar = i;
	setChanged();
	notifyObservers(string("lastloopbar"));
}

int Sequence::getLastLoopBar()
{
	if (lastLoopBarEnd) {
		return lastBar;
	}
	return lastLoopBar;
}

vector<weak_ptr<Track>> Sequence::getMetaTracks() {
	auto res = vector<weak_ptr<Track>>();
	for (auto& t : metaTracks) {
		res.push_back(t);
	}
	return res;
}

void Sequence::initMetaTracks()
{
	createClickTrack();
	createTempoChangeTrack();
	createMidiClockTrack();
}

void Sequence::createClickTrack()
{
	metaTracks[0]->removeEvents();
	auto bars = getLastBar() + 1;
	auto den = 0;
	auto denTicks = 0;

	auto countMetronomeScreen = dynamic_pointer_cast<CountMetronomeScreen>(Screens::getScreenComponent("count-metronome"));
	auto metronomeSoundScreen = dynamic_pointer_cast<MetronomeSoundScreen>(Screens::getScreenComponent("metronome-sound"));

	for (int i = 0; i < bars; i++)
	{
		den = denominators[i];
		denTicks = (int)(96 * (4.0 / den));
		int barStartPos = 0;
	
		for (auto k = 0; k < i; k++)
		{
			barStartPos += barLengths[k];
		}

		switch (countMetronomeScreen->getRate())
		{
		case 1:
			denTicks *= 2.0f / 3;
			break;
		case 2:
			denTicks *= 1.0f / 2;
			break;
		case 3:
			denTicks *= 1.0f / 3;
			break;
		case 4:
			denTicks *= 1.0f / 4;
			break;
		case 5:
			denTicks *= 1.0f / 6;
			break;
		case 6:
			denTicks *= 1.0f / 8;
			break;
		case 7:
			denTicks *= 1.0f / 12;
			break;
		}

		for (auto j = 0; j < barLengths[i]; j += denTicks)
		{
			auto n = dynamic_pointer_cast<NoteEvent>(metaTracks[0]->addEvent(barStartPos + j, "note").lock());
			n->setDuration(1);
		
			if (j == 0)
			{
				n->setVelocity(metronomeSoundScreen->getAccentVelo());
				n->setNote(metronomeSoundScreen->getAccentNote());
			}
			else
			{
				n->setVelocity(metronomeSoundScreen->getNormalVelo());
				n->setNote(metronomeSoundScreen->getNormalNote());
		
			}
		}
	}
	metaTracks[0]->sortEvents();
}

void Sequence::createMidiClockTrack()
{
	//(*metaTracks)[1]->getEvents()->clear();
	//(*metaTracks)[1]->addEvent(new MidiClockEvent(midi::ShortMessage::START));
	for (int i = 0; i < getLastTick(); i += 4) {
		// auto mcm = new MidiClockEvent(::javax::sound::midi::ShortMessage::TIMING_CLOCK);
		//mcm->setTick(i);
		//(*metaTracks)[1]->addEvent(mcm);
	}
}

void Sequence::createTempoChangeTrack()
{
	metaTracks[2]->removeEvents();
	auto tce = metaTracks[2]->addEvent(0, "tempo-change").lock();
	dynamic_pointer_cast<mpc::sequencer::TempoChangeEvent>(tce)->setStepNumber(0);
}

bool Sequence::isLoopEnabled()
{
	return loopEnabled;
}

void Sequence::setName(string s)
{
	name = s;
}

string Sequence::getName()
{
	if (!used) return string("(Unused)");
	return name;
}

void Sequence::setDeviceName(int i, string s)
{
	deviceNames[i] = s;
}

string Sequence::getDeviceName(int i)
{
	return deviceNames[i];
}

void Sequence::setLastBar(int i)
{
	if (i < 0 || i > 998) {
		return;
	}
	lastBar = i;
}

int Sequence::getLastBar()
{
	return lastBar;
}

void Sequence::setLoopEnabled(bool b)
{
	loopEnabled = b;
	setChanged();
	notifyObservers(string("loop"));
}

weak_ptr<Track> Sequence::getTrack(int i)
{
	return tracks[i];
}

void Sequence::setUsed(bool b)
{
	used = b;
}

bool Sequence::isUsed()
{
	return used;
}

void Sequence::init(int lastBarIndex)
{
	used = true;
	auto& userDefaults = mpc::ui::UserDefaults::instance();
	initialTempo = userDefaults.getTempo();
	loopEnabled = userDefaults.isLoopEnabled();
	for (auto& track : getTracks()) {
		auto lTrack = track.lock();
		lTrack->setDeviceNumber(userDefaults.getDeviceNumber());
		lTrack->setProgramChange(userDefaults.getPgm());
		lTrack->setBusNumber(userDefaults.getBus());
		lTrack->setVelocityRatio(userDefaults.getVeloRatio());
	}
	setLastBar(lastBarIndex);
	initMetaTracks();
	initLoop();
	setTimeSignature(0, getLastBar(), userDefaults.getTimeSig().getNumerator(), userDefaults.getTimeSig().getDenominator());
}

void Sequence::setTimeSignature(int firstBar, int tsLastBar, int num, int den)
{
	sequencer::SeqUtil::setTimeSignature(this, firstBar, tsLastBar, num, den);
}

void Sequence::setTimeSignature(int bar, int num, int den)
{
	sequencer::SeqUtil::setTimeSignature(this, bar, num, den);
}

vector<weak_ptr<Track>> Sequence::getTracks()
{
	auto res = vector<weak_ptr<Track>>();
	for (auto& t : tracks)
		res.push_back(t);
	return res;
}

vector<string> Sequence::getDeviceNames()
{
	return deviceNames;
}

void Sequence::setDeviceNames(vector<string> sa)
{
	deviceNames = sa;
}

vector<weak_ptr<TempoChangeEvent>> Sequence::getTempoChangeEvents()
{
	auto res = vector<weak_ptr<TempoChangeEvent>>();
	for (auto& t : metaTracks[2]->getEvents())
		res.push_back(dynamic_pointer_cast<TempoChangeEvent>(t.lock()));
	return res;
}

weak_ptr<TempoChangeEvent> Sequence::addTempoChangeEvent() {
	auto res = metaTracks[2]->addEvent(0, "tempo-change");
	return dynamic_pointer_cast<TempoChangeEvent>(res.lock());
}

BCMath Sequence::getInitialTempo()
{
	return initialTempo;
}

void Sequence::setInitialTempo(BCMath bd)
{
	auto str = to_string(bd.toDouble());
	
	if (str.find(".") == string::npos)
	{
		str += ".0";
	}
	
	auto length = (int)(str.find(".")) + 2;
	auto tempo = BCMath(str.substr(0, length));
	
	if (tempo.toDouble() < 30.0)
	{
		tempo = BCMath("30.0");
	}
	
	if (tempo.toDouble() > 300.0)
	{
		tempo = BCMath("300.0");
	}

	initialTempo = tempo;
	setChanged();
	notifyObservers(string("initial-tempo"));
}


void Sequence::removeTempoChangeEvent(int i) {
	metaTracks[2]->removeEvent(i);
}

void Sequence::removeTempoChangeEvent(weak_ptr<TempoChangeEvent> tce) {
	metaTracks[2]->removeEvent(tce);
}

bool Sequence::isTempoChangeOn()
{
	return tempoChangeOn;
}

void Sequence::setTempoChangeOn(bool b)
{
	tempoChangeOn = b;
	setChanged();
	notifyObservers(string("tempochangeon"));
}

int Sequence::getLastTick()
{
	int lastTick = 0;
	for (int i = 0; i < getLastBar() + 1; i++) {
		lastTick += barLengths[i];
	}
	return lastTick;
}

TimeSignature Sequence::getTimeSignature()
{
	auto ts = TimeSignature();
	int bar = Mpc::instance().getSequencer().lock()->getCurrentBarNumber();
	ts.setNumerator(numerators[bar]);
	ts.setDenominator(denominators[bar]);
	return ts;
}

void Sequence::sortTempoChangeEvents()
{
	metaTracks[2]->sortEvents();
	int tceCounter = 0;
	for (auto& e : metaTracks[2]->getEvents()) {
		auto tce = dynamic_pointer_cast<TempoChangeEvent>(e.lock());
		tce->setStepNumber(tceCounter);
		tceCounter++;
	}
}

void Sequence::sortTracks()
{
	sort(tracks.begin(), tracks.end(), trackIndexComparator);
}

void Sequence::purgeAllTracks()
{
	for (int i = 0; i < 64; i++) {
		purgeTrack(i);
	}
}

weak_ptr<Track> Sequence::purgeTrack(int i)
{
	tracks[i] = make_shared<Track>(this, i);
	tracks[i]->setName(defaultTrackNames[i]);
	return tracks[i];
}

int Sequence::getDenominator(int i)
{
	return denominators[i];
}

int Sequence::getNumerator(int i)
{
	return numerators[i];
}

vector<int>* Sequence::getBarLengths()
{
	return &barLengths;
}

void Sequence::setBarLengths(vector<int>& newBarLengths) {
	barLengths = newBarLengths;
}

void Sequence::deleteBars(int firstBar, int lBar)
{
	lBar++;

	int deleteFirstTick = 0;
	for (int i = 0; i < firstBar; i++)
		deleteFirstTick += barLengths[i];
	int deleteLastTick = deleteFirstTick;
	for (int i = firstBar; i < lBar; i++)
		deleteLastTick += barLengths[i];

	for (auto& t : tracks) {
		for (auto& e : t->getEvents()) {
			if (e.lock()->getTick() >= deleteFirstTick && e.lock()->getTick() < deleteLastTick)
				t->removeEvent(e);
		}
	}

	auto difference = lBar - firstBar;
	lastBar -= difference;
	int oldBarStartPos = 0;
	auto barCounter = 0;
	for (auto l : barLengths) {
		if (barCounter == lBar)	break;
		oldBarStartPos += l;
		barCounter++;
	}
	int newBarStartPos = 0;
	barCounter = 0;
	for (auto l : barLengths) {
		if (barCounter == firstBar)	break;
		newBarStartPos += l;
		barCounter++;
	}
	auto tickDifference = oldBarStartPos - newBarStartPos;
	for (int i = firstBar; i < 999; i++) {
		if (i + difference > 998) break;

		barLengths[i] = barLengths[i + difference];
		numerators[i] = numerators[i + difference];
		denominators[i] = denominators[i + difference];
	}
	for (auto& t : tracks) {
		if (t->getTrackIndex() >= 64 || t->getTrackIndex() == 65) continue;

		for (auto& event : t->getEvents()) {
			auto e = event.lock();
			if (e->getTick() >= oldBarStartPos) {
				e->setTick(e->getTick() - tickDifference);
			}
		}
	}

	if (firstLoopBar > lastBar) firstLoopBar = lastBar;
	if (lastLoopBar > lastBar) lastLoopBar = lastBar;
}

void Sequence::insertBars(int numberOfBars, int afterBar)
{
	lastBar += numberOfBars;
	for (int i = afterBar; i < 999; i++) {
		if (i + numberOfBars > 998)
			break;

		barLengths[i + numberOfBars] = barLengths[i];
		numerators[i + numberOfBars] = numerators[i];
		denominators[i + numberOfBars] = denominators[i];
	}
	for (int i = afterBar; i < afterBar + numberOfBars; i++) {
		barLengths[i] = 384;
		numerators[i] = 4;
		denominators[i] = 4;
	}
	int barStart = 0;
	auto barCounter = 0;
	for (auto l : barLengths) {
		if (barCounter == afterBar)
			break;

		barStart += l;
		barCounter++;
	}
	barCounter = 0;
	int newBarStart = 0;
	for (auto l : barLengths) {
		if (barCounter == afterBar + numberOfBars) {
			break;
		}

		newBarStart += l;
		barCounter++;
	}
	for (auto& t : tracks) {
		if (t->getTrackIndex() == 64 || t->getTrackIndex() == 65) continue;
		for (auto& event : t->getEvents()) {
			auto e = event.lock();
			if (e->getTick() >= barStart) {
				e->setTick(e->getTick() + (newBarStart - barStart));
			}
		}
	}
	initMetaTracks();
}

void Sequence::moveTrack(int source, int destination)
{
	if (source == destination) return;
	if (source > destination) {
		tracks[source]->setTrackIndex(destination);
		for (int i = destination; i < source; i++) {
			auto t = tracks[i];
			t->setTrackIndex(t->getTrackIndex() + 1);
		}
	}
	if (destination > source) {
		tracks[source]->setTrackIndex(destination);
		for (int i = source + 1; i <= destination; i++) {
			auto t = tracks[i];
			t->setTrackIndex(t->getTrackIndex() - 1);
		}
	}
	sortTracks();
}

bool Sequence::isLastLoopBarEnd()
{
	return lastLoopBarEnd;
}

int Sequence::getEventCount()
{
	auto counter = 0;
	for (auto& t : tracks) {
		if (t->getTrackIndex() > 63) break;
		counter += t->getEvents().size();
	}
	return counter;
}

void Sequence::initLoop()
{
	auto firstBar = getFirstLoopBar();
	auto lastBar = getLastLoopBar() + 1;
	int loopStart = 0;
	int loopEnd = 0;
	for (int i = 0; i < lastBar; i++) {
		if (i < firstBar) {
			loopStart += barLengths[i];
		}
		loopEnd += barLengths[i];
	}
	setLoopStart(loopStart);
	setLoopEnd(loopEnd);
}

vector<int>* Sequence::getNumerators()
{
	return &numerators;
}

vector<int>* Sequence::getDenominators()
{
	return &denominators;
}

void Sequence::setNumeratorsAndDenominators(vector<int>& numerators, vector<int>& denominators)
{
	this->numerators = numerators;
	this->denominators = denominators;
}

void Sequence::removeFirstMetronomeClick()
{
	metaTracks[0]->removeEvent(0);
}

int Sequence::getNoteEventCount()
{
	auto eventCounter = 0;
	for (int i = 0; i < 64; i++) {
		eventCounter += tracks[i]->getNoteEvents().size();
	}
	return eventCounter;
}

bool Sequence::trackIndexComparator(weak_ptr<Track> t0, weak_ptr<Track> t1)
{
	return t0.lock()->getTrackIndex() < t1.lock()->getTrackIndex();
}

int Sequence::getFirstTickOfBar(int index)
{
	int res = 0;
	for (int i = 0; i < index; i++) {
		res += barLengths[i];
	}
	return res;
}

int Sequence::getLastTickOfBar(int index)
{
	return getFirstTickOfBar(index) + barLengths[index] - 1;
}

void Sequence::resetTrackEventIndices(int tick)
{
	if (!isUsed())
	{
		return;
	}

	for (auto& t : tracks)
	{
		if (t->isUsed())
		{
			t->move(tick, tick);
		}
	}
	
	for (auto& t : metaTracks)
	{
		if (t->isUsed())
		{
			t->move(tick, tick);
		}
	}
}
