#include "Sequence.hpp"

#include <Mpc.hpp>

#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lcdgui/screens/UserScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;
using namespace std;

Sequence::Sequence(mpc::Mpc& _mpc)
	: mpc (_mpc), defaultTrackNames (_mpc.getSequencer().lock()->getDefaultTrackNames())
{
	for (int i = 0; i < 64; i++)
	{
		tracks.push_back(make_shared<Track>(mpc, this, i));
        weakTracks.push_back(tracks.back());
		tracks[i]->setName(defaultTrackNames[i]);
	}

	metaTracks.push_back(make_shared<Track>(mpc, this, 64));
	metaTracks.push_back(make_shared<Track>(mpc, this, 65));

    for (auto& mt : metaTracks)
        weakMetaTracks.push_back(mt);
    
	metaTracks[0]->setUsed(true);
	metaTracks[1]->setUsed(true);
	metaTracks[0]->setName("midiclock");
	metaTracks[1]->setName("tempo");

	auto userScreen = mpc.screens->get<UserScreen>("user");

	for (int i = 0; i < 33; i++)
		deviceNames[i] = userScreen->getDeviceName(i);
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

void Sequence::setFirstLoopBarIndex(int i)
{
	if (i < 0 || i > lastBarIndex)
		return;

	firstLoopBarIndex = i;
	
	notifyObservers(string("firstloopbar"));
	
	if (i > lastLoopBarIndex)
	{
		lastLoopBarIndex = i;
		notifyObservers(string("lastloopbar"));
	}
}

int Sequence::getFirstLoopBarIndex()
{
	return firstLoopBarIndex;
}

void Sequence::setLastLoopBarIndex(int i)
{
	if (i < 0)
		return;

	if (lastLoopBarEnd)
	{
		if (i < lastBarIndex)
		{
			lastLoopBarEnd = false;
			lastLoopBarIndex = lastBarIndex;
			
			notifyObservers(string("lastloopbar"));
			return;
		}
		else
		{
			return;
		}
	}
	else
	{
		if (i > lastBarIndex)
		{
			lastLoopBarEnd = true;	
			notifyObservers(string("lastloopbar"));
		}
		else
		{
			lastLoopBarIndex = i;
			
			notifyObservers(string("lastloopbar"));
		
			if (i < firstLoopBarIndex)
			{
				firstLoopBarIndex = i;
				notifyObservers(string("firstloopbar"));
			}
		}
	}

	lastLoopBarIndex = i;
	
	notifyObservers(string("lastloopbar"));
}

int Sequence::getLastLoopBarIndex()
{
	if (lastLoopBarEnd)
		return lastBarIndex;
	
	return lastLoopBarIndex;
}

vector<weak_ptr<Track>> Sequence::getMetaTracks()
{
    return weakMetaTracks;
}

void Sequence::initMetaTracks()
{
	createTempoChangeTrack();
	createMidiClockTrack();
}

void Sequence::createMidiClockTrack()
{
//    auto lastTick = getLastTick();
	//(*metaTracks)[1]->getEvents()->clear();
	//(*metaTracks)[1]->addEvent(new MidiClockEvent(midi::ShortMessage::START));
//	for (int i = 0; i < lastTick; i += 4) {
		// auto mcm = new MidiClockEvent(::javax::sound::midi::ShortMessage::TIMING_CLOCK);
		//mcm->setTick(i);
		//(*metaTracks)[1]->addEvent(mcm);
//	}
}

void Sequence::createTempoChangeTrack()
{
	metaTracks[1]->removeEvents();
	auto tce = metaTracks[1]->addEvent(0, "tempo-change");
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
	if (!used)
		return string("(Unused)");

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
	if (i < 0 || i > 998)
		return;
	
	lastBarIndex = i;
}

int Sequence::getLastBarIndex()
{
	return lastBarIndex;
}

void Sequence::setLoopEnabled(bool b)
{
	loopEnabled = b;
	
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

void Sequence::init(int newLastBarIndex)
{
	used = true;

	auto userScreen = mpc.screens->get<UserScreen>("user");
	initialTempo = userScreen->tempo;
	loopEnabled = userScreen->loop;

	for (auto& t : tracks)
	{
		t->setDeviceNumber(userScreen->device);
		t->setProgramChange(userScreen->pgm);
		t->setBusNumber(userScreen->bus);
		t->setVelocityRatio(userScreen->velo);
	}
	
	setLastBar(newLastBarIndex);
	initMetaTracks();
	initLoop();
    
	setTimeSignature(0, getLastBarIndex(), userScreen->timeSig.getNumerator(), userScreen->timeSig.getDenominator());
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
    return weakTracks;
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
	
	for (auto& t : metaTracks[1]->getEvents())
		res.push_back(dynamic_pointer_cast<TempoChangeEvent>(t.lock()));

	return res;
}

std::shared_ptr<TempoChangeEvent> Sequence::addTempoChangeEvent()
{
	auto res = metaTracks[1]->addEvent(0, "tempo-change");
	return dynamic_pointer_cast<TempoChangeEvent>(res);
}

double Sequence::getInitialTempo()
{
	return initialTempo;
}

void Sequence::setInitialTempo(const double newInitialTempo)
{	
	this->initialTempo = newInitialTempo;

	if (newInitialTempo < 30.0)
		this->initialTempo = 30.0;
	else if (newInitialTempo > 300.0)
		this->initialTempo = 300.0;
	
	notifyObservers(string("initial-tempo"));
}


void Sequence::removeTempoChangeEvent(int i)
{
	metaTracks[1]->removeEvent(i);
}

bool Sequence::isTempoChangeOn()
{
	return tempoChangeOn;
}

void Sequence::setTempoChangeOn(bool b)
{
	tempoChangeOn = b;
	notifyObservers(string("tempochangeon"));
}

int Sequence::getLastTick()
{
	int lastTick = 0;
	
	for (int i = 0; i < getLastBarIndex() + 1; i++)
		lastTick += barLengthsInTicks[i];
	
	return lastTick;
}

TimeSignature Sequence::getTimeSignature()
{
	auto ts = TimeSignature();
	int bar = mpc.getSequencer().lock()->getCurrentBarIndex();

	if (bar > lastBarIndex && bar != 0)
		bar--;

	ts.setNumerator(numerators[bar]);
	ts.setDenominator(denominators[bar]);

	return ts;
}

void Sequence::sortTempoChangeEvents()
{
	metaTracks[1]->sortEvents();
	int tceCounter = 0;

	for (auto& e : metaTracks[1]->getEvents())
	{
		auto tce = dynamic_pointer_cast<TempoChangeEvent>(e.lock());
		tce->setStepNumber(tceCounter);
		tceCounter++;
	}
}

void Sequence::purgeAllTracks()
{
	for (int i = 0; i < 64; i++)
		purgeTrack(i);
}

weak_ptr<Track> Sequence::purgeTrack(int i)
{
	tracks[i] = make_shared<Track>(mpc, this, i);
	tracks[i]->setName(defaultTrackNames[i]);
    weakTracks[i] = tracks[i];
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

vector<int>& Sequence::getBarLengthsInTicks()
{
	return barLengthsInTicks;
}

void Sequence::setBarLengths(vector<int>& newBarLengths)
{
    barLengthsInTicks = newBarLengths;
}

void Sequence::deleteBars(int firstBar, int _lastBar)
{
	if (lastBarIndex == -1)
		return;

	_lastBar++;

	int deleteFirstTick = 0;

	for (int i = 0; i < firstBar; i++)
		deleteFirstTick += barLengthsInTicks[i];

	int deleteLastTick = deleteFirstTick;
	
	for (int i = firstBar; i < _lastBar; i++)
		deleteLastTick += barLengthsInTicks[i];

	for (auto& t : tracks)
	{
		for (auto& e : t->getEvents())
		{
			if (e.lock()->getTick() >= deleteFirstTick && e.lock()->getTick() < deleteLastTick)
				t->removeEvent(e);
		}
	}

	auto difference = _lastBar - firstBar;
	lastBarIndex -= difference;
	int oldBarStartPos = 0;
	auto barCounter = 0;

	for (auto l : barLengthsInTicks)
	{
		if (barCounter == _lastBar)
			break;

		oldBarStartPos += l;
		barCounter++;
	}

	int newBarStartPos = 0;
	barCounter = 0;
	
	for (auto l : barLengthsInTicks)
	{
		if (barCounter == firstBar)
			break;

		newBarStartPos += l;
		barCounter++;
	}

	auto tickDifference = oldBarStartPos - newBarStartPos;
	
	for (int i = firstBar; i < 999; i++)
	{
		if (i + difference > 998)
			break;

        barLengthsInTicks[i] = barLengthsInTicks[i + difference];
		numerators[i] = numerators[i + difference];
		denominators[i] = denominators[i + difference];
	}

	for (auto& t : tracks)
	{
		if (t->getIndex() >= 64 || t->getIndex() == 65)
			continue;

		for (auto& event : t->getEvents())
		{
			auto e = event.lock();
			
			if (e->getTick() >= oldBarStartPos)
				e->setTick(e->getTick() - tickDifference);
		}
	}

	if (firstLoopBarIndex > lastBarIndex)
		firstLoopBarIndex = lastBarIndex;

	if (lastLoopBarIndex > lastBarIndex)
		lastLoopBarIndex = lastBarIndex;

	if (lastBarIndex  == -1)
		setUsed(false);

	createMidiClockTrack();
}

void Sequence::insertBars(int barCount, int afterBar)
{
    const bool isAppending = afterBar == lastBarIndex;

	lastBarIndex += barCount;

	for (int i = afterBar; i < 999; i++)
	{
		if (i + barCount > 998)
			break;

        barLengthsInTicks[i + barCount] = barLengthsInTicks[i];
		numerators[i + barCount] = numerators[i];
		denominators[i + barCount] = denominators[i];
	}

	for (int i = afterBar; i < afterBar + barCount; i++)
	{
        barLengthsInTicks[i] = 384;
		numerators[i] = 4;
		denominators[i] = 4;
	}
	
	int barStart = 0;
	auto barCounter = 0;
	
	for (auto l : barLengthsInTicks)
	{
		if (barCounter == afterBar)
			break;

		barStart += l;
		barCounter++;
	}

	barCounter = 0;
	int newBarStart = 0;
	
	for (auto l : barLengthsInTicks)
	{
		if (barCounter == afterBar + barCount)
			break;

		newBarStart += l;
		barCounter++;
	}

    if (!isAppending)
    {
        for (auto& t : tracks)
        {
            if (t->getIndex() == 64 || t->getIndex() == 65)
                continue;

            for (auto& event : t->getEvents())
            {
                auto e = event.lock();

                if (e->getTick() >= barStart)
                    e->setTick(e->getTick() + (newBarStart - barStart));
            }
        }
    }

	createMidiClockTrack();

	if (lastBarIndex != -1)
		setUsed(true);

	notifyObservers(string("numberofbars"));
	notifyObservers(string("tempo"));
	notifyObservers(string("timesignature"));
}

void Sequence::moveTrack(int source, int destination)
{
	if (source == destination)
		return;

	if (source > destination)
	{
		tracks[source]->setTrackIndex(destination);
	
		for (int i = destination; i < source; i++)
		{
			auto t = tracks[i];
			t->setTrackIndex(t->getIndex() + 1);
		}
	}

	if (destination > source)
	{
		tracks[source]->setTrackIndex(destination);
		
		for (int i = source + 1; i <= destination; i++)
		{
			auto t = tracks[i];
			t->setTrackIndex(t->getIndex() - 1);
		}
	}
    
    sort(begin(tracks), end(tracks), trackIndexComparator);
    sort(begin(weakTracks), end(weakTracks), trackIndexComparator);
}

bool Sequence::isLastLoopBarEnd()
{
	return lastLoopBarEnd;
}

int Sequence::getEventCount()
{
	auto counter = 0;
	
	for (auto& t : tracks)
	{
		if (t->getIndex() > 63)
			break;
	
		counter += t->getEvents().size();
	}

	return counter;
}

void Sequence::initLoop()
{
	if (firstLoopBarIndex == -1 && lastBarIndex >= 0)
		firstLoopBarIndex = 0;

	if (lastLoopBarIndex == -1 && lastBarIndex >= 0)
		lastLoopBarIndex = lastBarIndex;

	auto firstBar = getFirstLoopBarIndex();
	auto lastBar = getLastLoopBarIndex() + 1;
	int newLoopStart = 0;
	int newLoopEnd = 0;

	for (int i = 0; i < lastBar; i++)
	{
		if (i < firstBar)
            newLoopStart += barLengthsInTicks[i];

        newLoopEnd += barLengthsInTicks[i];
	}

	setLoopStart(newLoopStart);
	setLoopEnd(newLoopEnd);
}

vector<int>* Sequence::getNumerators()
{
	return &numerators;
}

vector<int>* Sequence::getDenominators()
{
	return &denominators;
}

void Sequence::setNumeratorsAndDenominators(vector<int>& newNumerators, vector<int>& newDenominators)
{
	this->numerators = newNumerators;
	this->denominators = newDenominators;
}

bool Sequence::trackIndexComparator(weak_ptr<Track> t0, weak_ptr<Track> t1)
{
	return t0.lock()->getIndex() < t1.lock()->getIndex();
}

int Sequence::getFirstTickOfBeat(int bar, int beat)
{
	auto barStart = getFirstTickOfBar(bar);
	auto den = denominators[bar];
	auto beatTicks = (int)(96 * (4.0 / den));
	return barStart + (beat * beatTicks);
}

int Sequence::getFirstTickOfBar(int index)
{
	int res = 0;

	for (int i = 0; i < index; i++)
		res += barLengthsInTicks[i];

	return res;
}

int Sequence::getLastTickOfBar(int index)
{
	return getFirstTickOfBar(index) + barLengthsInTicks[index] - 1;
}

void Sequence::resetTrackEventIndices(int tick)
{
	if (!isUsed() || tick > getLastTick())
		return;

	for (auto& t : tracks)
	{
		if (t->isUsed())
			t->move(tick, tick);
	}
	
	for (auto& t : metaTracks)
	{
		if (t->isUsed())
			t->move(tick, tick);
	}
}
