#include "Sequencer.hpp"
#include "lcdgui/screens/SyncScreen.hpp"

#include <Mpc.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/MpcMidiOutput.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>

#include <lcdgui/screens/SongScreen.hpp>
#include <lcdgui/screens/SecondSeqScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/CountMetronomeScreen.hpp>
#include <lcdgui/screens/window/IgnoreTempoChangeScreen.hpp>
#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>

#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/FrameSeq.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Song.hpp>
#include <sequencer/Step.hpp>

// ctoot
#include <engine/audio/server/NonRealTimeAudioServer.hpp>
#include <engine/mpc/MpcSoundPlayerChannel.hpp>

// moduru
#include <System.hpp>
#include <lang/StrUtil.hpp>

#include <algorithm>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace moduru::lang;

Sequencer::Sequencer(mpc::Mpc& mpc)
	: mpc (mpc)
{
}

void Sequencer::init()
{
	lastTap = moduru::System::currentTimeMillis();
	nextSq = -1;

	auto userScreen = mpc.screens->get<UserScreen>("user");
	defaultSequenceName = StrUtil::trim(userScreen->sequenceName);
	
	for (int i = 0; i < 64; i++)
	{
        std::string name = "Track-";
		name = name.append(StrUtil::padLeft(std::to_string(i + 1), "0", 2));
		defaultTrackNames.push_back(name);
	}

	activeTrackIndex = 0;

	recordingModeMulti = userScreen->recordingModeMulti;

	soloEnabled = false;
	tempoSourceSequenceEnabled = true;
	countEnabled = true;
	recording = false;
	
	tempo = userScreen->tempo;
	tempo = 120.0;

	metronomeOnly = false;
	metronomeSeq = nullptr;
	activeSequenceIndex = 0;
	currentlyPlayingSequenceIndex = 0;
	songMode = false;

	purgeAllSequences();
	
	for (int i = 0; i < 20; i++)
		songs[i] = std::make_shared<Song>();
}

void Sequencer::deleteSong(int i)
{
	songs[i] = std::make_shared<Song>();
}

int Sequencer::getActiveSequenceIndex()
{
	return activeSequenceIndex;
}

std::shared_ptr<Track> Sequencer::getActiveTrack()
{
	if (!getActiveSequence())
		return {};

	return getActiveSequence()->getTrack(activeTrackIndex);
}

void Sequencer::playToTick(int targetTick)
{
	auto seqIndex = songMode ? getSongSequenceIndex() : currentlyPlayingSequenceIndex;
	auto seq = metronomeOnly ? metronomeSeq.get() : sequences[seqIndex].get();
	auto secondSequenceScreen = mpc.screens->get<SecondSeqScreen>("second-seq");

	for (int i = 0; i < 2; i++)
	{
		if (i == 1)
		{
			if (!secondSequenceEnabled || metronomeOnly || secondSequenceScreen->sq == seqIndex) // Real 2KXL would play all events twice for the last clause
				break;

			seq = sequences[secondSequenceScreen->sq].get();

			if (!seq->isUsed())
				break;
		}

		for (auto& track : seq->getTracks())
        {
			while (track->getNextTick() <= targetTick)
				track->playNext();
		}

		for (auto& track : seq->getMetaTracks())
        {
			while (track->getNextTick() <= targetTick)
				track->playNext();
		}
	}
}

std::vector<int> Sequencer::getTickValues() {
	return TICK_VALUES;
}

void Sequencer::setTempo(double newTempo)
{
	if (newTempo < 30.0)
		newTempo = 30.0;
	else if (newTempo > 300.0)
		newTempo = 300.0;

    auto s = getActiveSequence();
	auto tce = getCurrentTempoChangeEvent();

	if (!s || !s->isUsed() || !tempoSourceSequenceEnabled)
	{
		if (tce)
		{
			auto candidate = newTempo / (tce->getRatio() * 0.001);

			if (candidate < 30.0)
				candidate = 30.0;
			else if (candidate > 300.0)
				candidate = 300.0;

			tempo = candidate;
		}
		else
		{
			tempo = newTempo;
		}
		notifyObservers(std::string("tempo"));
		return;
	}

	if (tce && tce->getTick() == 0 && s->isTempoChangeOn())
	{
		s->setInitialTempo(newTempo / (tce->getRatio() * 0.001));
	}
	else if (s->isTempoChangeOn())
	{
		auto initialTempo = s->getInitialTempo();
		auto ratio = newTempo / initialTempo;
		tce->setRatio((int) round(ratio * 1000.0));
	}
	else
	{
		s->setInitialTempo(newTempo);
	}

	notifyObservers(std::string("tempo"));
}

double Sequencer::getTempo()
{
	if (!isPlaying() && !getActiveSequence()->isUsed())
		return tempo;

	auto seq = getActiveSequence();
	auto tce = getCurrentTempoChangeEvent();

	if (mpc.getLayeredScreen()->getCurrentScreenName().compare("song") == 0)
	{
		if (!seq->isUsed())
			return 120.0;
	}

	if (tempoSourceSequenceEnabled)
	{
		auto ignoreTempoChangeScreen = mpc.screens->get<IgnoreTempoChangeScreen>("ignore-tempo-change");

		if (seq->isTempoChangeOn() || (songMode && !ignoreTempoChangeScreen->ignore))
		{
			if (tce)
				return tce->getTempo();
		}

		return getActiveSequence()->getInitialTempo();
	}

	if (seq->isTempoChangeOn() && tce)
		return tempo * tce->getRatio() * 0.001;
	
	return tempo;
}

std::shared_ptr<TempoChangeEvent> Sequencer::getCurrentTempoChangeEvent()
{
	auto index = -1;
	auto s = getActiveSequence();

	if (!s->isUsed())
		return {};

	for (auto& tce : s->getTempoChangeEvents())
	{
		if (getTickPosition() >= tce->getTick())
			index++;
		else
			break;
	}
	
	if (index == -1)
		index++;

	return s->getTempoChangeEvents()[index];
}

bool Sequencer::isTempoSourceSequenceEnabled()
{
    return tempoSourceSequenceEnabled;
}

void Sequencer::setTempoSourceSequence(bool b)
{
	tempoSourceSequenceEnabled = b;
	
	notifyObservers(std::string("tempo-source"));
	notifyObservers(std::string("tempo"));
}

bool Sequencer::isRecordingOrOverdubbing()
{
    return recording || overdubbing;
}

bool Sequencer::isRecording()
{
    return recording;
}

bool Sequencer::isSoloEnabled()
{
    return soloEnabled;
}

void Sequencer::setSoloEnabled(bool b)
{
	if (soloEnabled == b)
		return;

    soloEnabled = b;

    if (soloEnabled)
    {
        for (int i = 0; i < 4; i++)
        {
            mpc.getDrum(i).allNotesOff();
        }
    }

    notifyObservers(std::string("soloenabled"));
}

std::shared_ptr<Sequence> Sequencer::getSequence(int i)
{
    return sequences[i];
}

std::string Sequencer::getDefaultSequenceName()
{
	return defaultSequenceName;
}

void Sequencer::setDefaultSequenceName(std::string s)
{
    defaultSequenceName = s;
}

void Sequencer::setActiveSequenceIndex(int i)
{
	if (i < 0 || i > 98)
		return;

	activeSequenceIndex = i;
	
	if (!isPlaying())
	{
		position = 0;
		notifyTimeDisplay();
	}
	
	notifyObservers(std::string("seqnumbername"));
	notifyObservers(std::string("timesignature"));
	notifyObservers(std::string("numberofbars"));
	notifyObservers(std::string("tempo"));
	notifyObservers(std::string("loop"));
	notifyTrack();
}

bool Sequencer::isCountEnabled()
{
    return countEnabled;
}

void Sequencer::setCountEnabled(bool b)
{
    countEnabled = b;
    
    notifyObservers(std::string("count"));
}

void Sequencer::setTimeDisplayStyle(int i)
{
    timeDisplayStyle = i;
}

int Sequencer::getTimeDisplayStyle()
{
    return timeDisplayStyle;
}

void Sequencer::setRecordingModeMulti(bool b)
{
    recordingModeMulti = b;
    
    notifyObservers(std::string("recordingmode"));
}

bool Sequencer::isRecordingModeMulti()
{
    return recordingModeMulti;
}

int Sequencer::getActiveTrackIndex()
{
    return activeTrackIndex;
}

void Sequencer::trackUp()
{
    if(activeTrackIndex == 63) return;
    activeTrackIndex++;
    notifyTrack();
}

void Sequencer::trackDown()
{
    if(activeTrackIndex == 0) return;
    activeTrackIndex--;
    notifyTrack();
}

bool Sequencer::isPlaying()
{
	auto ams = mpc.getAudioMidiServices();
	auto frameSequencer = ams->getFrameSequencer();
	auto server = ams->getAudioServer();
	
	if (!server->isRunning() || !frameSequencer)
		return false;

	return !metronomeOnly && ams->getFrameSequencer()->isRunning();
}

void Sequencer::play(bool fromStart)
{
	if (isPlaying())
		return;

    endOfSong = false;
	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto currentSong = songs[songScreen->getActiveSongIndex()];
    
	std::shared_ptr<Step> currentStep;

	if (songMode)
	{
		if (!currentSong->isUsed())
			return;

		if (fromStart)
			songScreen->setOffset(-1);
		
		if (songScreen->getOffset() + 1 > currentSong->getStepCount() - 1)
			return;
		
		int step = songScreen->getOffset() + 1;
		
		if (step > currentSong->getStepCount())
			step = currentSong->getStepCount() - 1;
		
		currentStep = currentSong->getStep(step).lock();

		if (!sequences[currentStep->getSequence()]->isUsed())
			return;
	}

	move(position);
    
	currentlyPlayingSequenceIndex = activeSequenceIndex;

	auto countMetronomeScreen = mpc.screens->get<CountMetronomeScreen>("count-metronome");
	auto countInMode = countMetronomeScreen->getCountInMode();

    if (!countEnabled || countInMode == 0 || (countInMode == 1 && !isRecordingOrOverdubbing()))
	{
		if (fromStart)
			move(0);
    }
	
	auto s = getActiveSequence();
	
	if (countEnabled && !songMode)
	{
		if (countInMode == 2 || (countInMode == 1 && (isRecordingOrOverdubbing())))
		{
            if (fromStart)
			    move(s->getLoopStart());
            else
                move(s->getFirstTickOfBar(getCurrentBarIndex()));

            countInStartPos = position;
            countInEndPos = s->getLastTickOfBar(getCurrentBarIndex());

			countingIn = true;
		}
	}

	auto hw = mpc.getHardware();
	
	if (!songMode)
	{
		if (!s->isUsed())
		{
			recording = false;
			overdubbing = false;
			return;
		}

		s->initLoop();

		if (recording || overdubbing) {
			auto copy = copySequence(s);
			undoPlaceHolder.swap(copy);
			undoSeqAvailable = true;
		}

	}

	auto ams = mpc.getAudioMidiServices();

	if (ams->isBouncePrepared())
	{
		ams->startBouncing();
	}
	else
	{
		ams->getFrameSequencer()->start();
	}

    notifyObservers(std::string("play"));
}

void Sequencer::undoSeq()
{
	if (isPlaying())
		return;
	
	if (!undoPlaceHolder)
		return;

	auto s = copySequence(undoPlaceHolder);
	auto copy = copySequence(sequences[activeSequenceIndex]);	
	undoPlaceHolder.swap(copy);

	sequences[activeSequenceIndex].swap(s);
	
	sequences[activeSequenceIndex]->resetTrackEventIndices(position);

	undoSeqAvailable = !undoSeqAvailable;
	auto hw = mpc.getHardware();

	setActiveSequenceIndex(getActiveSequenceIndex()); // Shortcut to notifying SequencerObserver
}

bool Sequencer::isUndoSeqAvailable()
{
    return undoSeqAvailable;
}

void Sequencer::playFromStart()
{
	if (isPlaying())
		return;

	play(true);
}

void Sequencer::play()
{
	if (isPlaying())
		return;

	play(false);
}

void Sequencer::rec()
{
	if (isPlaying())
		return;

	recording = true;
	
	play(false);
}

void Sequencer::recFromStart()
{
	if (isPlaying())
		return;

	recording = true;
	play(true);
}

void Sequencer::overdub()
{
	if (isPlaying())
		return;

	overdubbing = true;
	play(false);
}

void Sequencer::switchRecordToOverDub()
{
	if (!recording)
		return;

	recording = false;
	overdubbing = true;
}

void Sequencer::overdubFromStart()
{
	if (isPlaying())
		return;

	overdubbing = true;
	play(true);
}

void Sequencer::stop()
{
	stop(-1);
}

void Sequencer::stop(int tick)
{
	auto ams = mpc.getAudioMidiServices();
	bool bouncing = ams->isBouncing();

	if (!isPlaying() && !bouncing)
	{
		if (position != 0)
        {
            setBar(0); // real 2kxl doesn't do this
        }
        return;
    }

    playedStepRepetitions = 0;
	songMode = false;

	lastNotifiedBar = -1;
	lastNotifiedBeat = -1;
	lastNotifiedClock = -1;

    auto s1 = getActiveSequence();
	auto s2 = getCurrentlyPlayingSequence();
	auto pos = getTickPosition();
	
	if (pos > s1->getLastTick())
		pos = s1->getLastTick();

	int frameOffset = tick == -1 ? 0 : ams->getFrameSequencer()->getEventFrameOffset();
	ams->getFrameSequencer()->stop();
	
    auto notifynextsq = false;
	
	if (nextSq != -1)
	{
		notifynextsq = true;
		nextSq = -1;

		// This is called from the audio thread, should be called from the UI thread instead.
		// stop() is called by FrameSeq.
		mpc.getLayeredScreen()->setFocus("sq");
	}

    recording = false;
    overdubbing = false;

    if (countingIn)
    {
        pos = countInStartPos;
        countInStartPos = -1;
        countInEndPos = -1;
        countingIn = false;
    }

    move(pos);

	if (!bouncing)
		mpc.getSampler()->stopAllVoices(frameOffset);

	for (int i = 0; i < 16; i++)
    {
        auto pad = mpc.getHardware()->getPad(i);
        pad->release();
        pad->notifyObservers(255);
    }

    if (notifynextsq)
        notifyObservers(std::string("nextsqoff"));
    	
	auto songScreen = mpc.screens->get<SongScreen>("song");

    if (endOfSong)
		songScreen->setOffset(songScreen->getOffset() + 1);
	
	auto vmpcDirectToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");

	if (bouncing && vmpcDirectToDiskRecorderScreen->getRecord() != 4)
		ams->stopBouncing();
	
	notifyObservers(std::string("stop"));
}

bool Sequencer::isCountingIn()
{
    return countingIn;
}

void Sequencer::setCountingIn(bool b)
{
    countingIn = b;

    if (!countingIn)
    {
        countInStartPos = -1;
        countInEndPos = -1;
    }
}

void Sequencer::notifyTrack()
{
    
    notifyObservers(std::string("tracknumbername"));
    
    notifyObservers(std::string("trackon"));
    
    notifyObservers(std::string("programchange"));
    
    notifyObservers(std::string("velocityratio"));
    
    notifyObservers(std::string("bus"));
    
    notifyObservers(std::string("device"));
    
    notifyObservers(std::string("devicename"));
}

void Sequencer::setSequence(int i, std::shared_ptr<Sequence> s)
{
	sequences[i].swap(s);
	sequences[i]->resetTrackEventIndices(position);
}

void Sequencer::purgeAllSequences()
{
	for (int i = 0; i < 99; i++) {
		purgeSequence(i);
	}
    activeSequenceIndex = 0;
}

void Sequencer::purgeSequence(int i) {
	sequences[i].reset();
	auto sequence = std::make_shared<Sequence>(mpc);
	sequences[i].swap(sequence);
	sequences[i]->resetTrackEventIndices(position);
	std::string res = defaultSequenceName;
	res.append(StrUtil::padLeft(std::to_string(i + 1), "0", 2));
	sequences[i]->setName(res);
}

void Sequencer::copySequence(int source, int destination)
{
	auto copy = copySequence(sequences[source]);
	sequences[destination].swap(copy);
	sequences[destination]->resetTrackEventIndices(position);
	sequences[destination]->initLoop();
}

void Sequencer::copySequenceParameters(const int source, const int dest)
{
	copySequenceParameters(sequences[source], sequences[dest]);
}

std::shared_ptr<Sequence> Sequencer::copySequence(std::shared_ptr<Sequence> source)
{
	auto copy = std::make_shared<Sequence>(mpc);
	copy->init(source->getLastBarIndex());
	copySequenceParameters(source, copy);
	
	for (int i = 0; i < 64; i++)
		copyTrack(source->getTrack(i), copy->getTrack(i));

	for (int i = 0; i < source->getMetaTracks().size(); i++)
	{
		copy->getMetaTracks()[i]->removeEvents();
		copyTrack(source->getMetaTracks()[i], copy->getMetaTracks()[i]);
	}

	for (auto& event : copy->getTempoChangeEvents())
		event->setParent(copy.get());

	return copy;
}

void Sequencer::copySequenceParameters(std::shared_ptr<Sequence> source, std::shared_ptr<Sequence> dest)
{
	dest->setName(source->getName());
	dest->setLoopEnabled(source->isLoopEnabled());
	dest->setUsed(source->isUsed());
	dest->setDeviceNames(source->getDeviceNames());
	dest->setInitialTempo(source->getInitialTempo());
	dest->setBarLengths(source->getBarLengthsInTicks());
	dest->setNumeratorsAndDenominators(source->getNumerators(), source->getDenominators());
	dest->setLoopStart(source->getLoopStart());
	dest->setLoopEnd(source->getLoopEnd());
    copyTempoChangeEvents(source, dest);
}

void Sequencer::copyTempoChangeEvents(std::shared_ptr<Sequence> src, std::shared_ptr<Sequence> dst) {
	for (auto& e1 : src->getTempoChangeEvents())
    {
        e1->CopyValuesTo(dst->addTempoChangeEvent());
    }
}

void Sequencer::copyTrack(int sourceTrackIndex, int destinationTrackIndex, int sourceSequenceIndex, int destinationSequenceIndex)
{
	if (sourceSequenceIndex == destinationSequenceIndex && sourceTrackIndex == destinationTrackIndex)
		return;

	auto src = sequences[sourceSequenceIndex]->getTrack(sourceTrackIndex);
	auto dest = sequences[destinationSequenceIndex]->purgeTrack(destinationTrackIndex);
	copyTrack(src, dest);
}

void Sequencer::copySong(const int source, const int dest)
{
	if (source == dest)
		return;

	auto s0 = songs[source];

	if (!s0->isUsed())
		return;

	deleteSong(dest);

	auto s1 = songs[dest];

	s1->setUsed(true);

	for (int i = 0; i < s0->getStepCount(); i++)
	{
		s1->insertStep(i);
		auto step = s1->getStep(i).lock();
		step->setRepeats(s0->getStep(i).lock()->getRepeats());
		step->setSequence(s0->getStep(i).lock()->getSequence());
	}

	s1->setFirstStep(s0->getFirstStep());
	s1->setLastStep(s0->getLastStep());
	s1->setName(s0->getName());
	s1->setLoopEnabled(s0->isLoopEnabled());
}

void Sequencer::copyTrack(std::shared_ptr<Track> src, std::shared_ptr<Track> dest)
{
    if (src == dest) return;

	dest->setTrackIndex(src->getIndex());
	
	for (auto& e : src->getEvents())
	{
        dest->cloneEventIntoTrack(e, e->getTick());
	}

	copyTrackParameters(src, dest);
}

void Sequencer::copyTrackParameters(std::shared_ptr<Track> source, std::shared_ptr<Track> dest)
{
	dest->setUsed(source->isUsed());
    dest->setOn(source->isOn());
    dest->setDeviceIndex(source->getDeviceIndex());
    dest->setBusNumber(source->getBus());
    dest->setVelocityRatio(source->getVelocityRatio());
    dest->setProgramChange(source->getProgramChange());
    dest->setName(source->getName());
}

std::string Sequencer::getDefaultTrackName(int i)
{
	return defaultTrackNames[i];
}

std::vector<std::string>& Sequencer::getDefaultTrackNames()
{
	return defaultTrackNames;
}

void Sequencer::setDefaultTrackName(std::string s, int i)
{
	defaultTrackNames[i] = s;
}

int Sequencer::getCurrentBarIndex()
{
	auto s = isPlaying() ? getCurrentlyPlayingSequence() : getActiveSequence();
	auto pos = getTickPosition();

	if (pos == s->getLastTick())
		return s->getLastBarIndex() + 1;
		
	auto& barLengths = s->getBarLengthsInTicks();
	
	int tickCounter = 0;
	
	for (int i = 0; i < 999; i++)
	{
		if (i > s->getLastBarIndex())
			return 0; // Should not happen
	
		tickCounter += barLengths[i];
		
		if (tickCounter > pos)
			return i;
	}

	return 0;
}

int Sequencer::getCurrentBeatIndex()
{
	auto s = isPlaying() ? getCurrentlyPlayingSequence() : getActiveSequence();
	auto pos = getTickPosition();
	if (pos == s->getLastTick()) return 0;
	auto index = pos;
	if (isPlaying() && !countingIn) {
		index = getTickPosition();
		if (index > s->getLastTick()) {
			index %= s->getLastTick();
		}
	}

	auto ts = s->getTimeSignature();
	auto den = ts.getDenominator();
	auto denTicks = 96 * (4.0 / den);

	if (index == 0) return 0;

	int barStartPos = 0;
	auto barCounter = 0;

	const auto currentBarIndex = getCurrentBarIndex();

	for (auto& l : s->getBarLengthsInTicks())
	{
		if (barCounter == currentBarIndex)
			break;
	
		barStartPos += l;
		barCounter++;
	}

	auto beatIndex = (int) floor((index - barStartPos) / denTicks);
	return beatIndex;
}

int Sequencer::getCurrentClockNumber()
{
	auto s = isPlaying() ? getCurrentlyPlayingSequence() : getActiveSequence();

	auto clock = getTickPosition();
	
	if (clock == s->getLastTick())
		return 0;
		
	if (isPlaying() && !countingIn)
	{
		if (clock > s->getLastTick())
			clock %= s->getLastTick();
	}

	auto ts = s->getTimeSignature();
	auto den = ts.getDenominator();
	auto denTicks = 96 * (4.0 / den);

	if (clock == 0)
		return 0;

	auto barCounter = 0;
	auto currentBarIndex = getCurrentBarIndex();

	for (auto& l : s->getBarLengthsInTicks())
	{
		if (barCounter == currentBarIndex)
			break;

		clock -= l;
		barCounter++;
	}
	
	auto currentBeatIndex = getCurrentBeatIndex();

	for (int i = 0; i < currentBeatIndex; i++)
		clock -= denTicks;

	return clock;
}

void Sequencer::setBar(int i)
{
	if (isPlaying())
		return;

	if (i < 0)
	{
		move(0);
		return;
	}

	auto s = getActiveSequence();
	
	if (i > s->getLastBarIndex() + 1)
		return;

	if (s->getLastBarIndex() == 998 && i > 998)
		return;

	auto ts = s->getTimeSignature();
	auto den = ts.getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	
	if (i != s->getLastBarIndex() + 1)
	{
		ts.setNumerator(s->getNumerator(i));
		ts.setDenominator(s->getDenominator(i));
	}

	auto& barLengths = s->getBarLengthsInTicks();
	auto currentClock = getCurrentClockNumber();
	auto currentBeat = getCurrentBeatIndex();
	int pos = 0;
	auto barCounter = 0;
	
	for (auto& l : barLengths)
	{
		if (barCounter == i)
			break;

		pos += l;
		barCounter++;
	}

	pos += currentBeat * denTicks;
	pos += currentClock;
	
	if (pos > s->getLastTick())
		pos = s->getLastTick();

	move(pos);
	
	notifyObservers(std::string("timesignature"));
	setBeat(0);
	setClock(0);
}

void Sequencer::setBeat(int i)
{
	if (i < 0 || isPlaying())
		return;

	auto s = getActiveSequence();
	auto pos = getTickPosition();
	
	if (pos == s->getLastTick())
		return;
	
	auto ts = s->getTimeSignature();
	auto difference = i - getCurrentBeatIndex();
	auto num = ts.getNumerator();
	
	if (i >= num)
		return;

	auto den = ts.getDenominator();
	auto denTicks = 96 * (4.0 / den);
	pos += difference * denTicks;
	move(pos);
}

void Sequencer::setClock(int i)
{
	if (i < 0 || isPlaying())
		return;

	auto s = getActiveSequence();
	int pos = getTickPosition();
	
	if (pos == s->getLastTick())
		return;
	
	getCurrentClockNumber();
	int difference = i - getCurrentClockNumber();
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = 96 * (4.0 / den);

	if (i > denTicks - 1)
		return;

	if (pos + difference > s->getLastTick())
		return;

	pos += difference;
	move(pos);
}

int Sequencer::getLoopEnd()
{
    return getActiveSequence()->getLoopEnd();
}

std::shared_ptr<Sequence> Sequencer::getActiveSequence()
{
	auto songScreen = mpc.screens->get<SongScreen>("song");

	if (songMode && songs[songScreen->getActiveSongIndex()]->getStepCount() != 0)
		return sequences[getSongSequenceIndex() >= 0 ? getSongSequenceIndex() : activeSequenceIndex];

	return sequences[activeSequenceIndex];
}

int Sequencer::getUsedSequenceCount()
{
    return getUsedSequences().size();
}

std::vector<std::shared_ptr<Sequence>> Sequencer::getUsedSequences()
{
	std::vector<std::shared_ptr<Sequence>> usedSeqs;

	for (auto s : sequences)
	{
		if (s->isUsed())
			usedSeqs.push_back(s);
	}

    return usedSeqs;
}

std::vector<int> Sequencer::getUsedSequenceIndexes()
{
	std::vector<int> usedSeqs;

	for (int i = 0; i < 99; i++)
	{
        auto s = sequences[i];
	
		if (s->isUsed())
			usedSeqs.push_back(i);
    }
	return usedSeqs;
}

void Sequencer::goToPreviousEvent()
{
    auto s = getActiveSequence();
    auto t = s->getTrack(getActiveTrackIndex());

    auto newPos = 0;
    auto events = t->getEvents();
    std::reverse(events.begin(), events.end());

    for (auto& e : events)
    {
        if (e->getTick() < getTickPosition())
        {
            newPos = e->getTick();
            break;
        }
    }

    move(newPos);
}

void Sequencer::goToNextEvent()
{
	auto s = getActiveSequence();
	auto t = s->getTrack(getActiveTrackIndex());

    auto newPos = s->getLastTick();

    for (auto& e : t->getEvents())
    {
        if (e->getTick() > getTickPosition())
        {
            newPos = e->getTick();
            break;
        }
    }

    move(newPos);
}

void Sequencer::notifyTimeDisplay()
{
	notifyObservers(std::string("bar"));
	notifyObservers(std::string("beat"));
	notifyObservers(std::string("clock"));
}

void Sequencer::notifyTimeDisplayRealtime()
{
	int bar = getCurrentBarIndex();
	int beat = getCurrentBeatIndex();
	int clock = getCurrentClockNumber();
	
	if (lastNotifiedBar != bar) {
		
		notifyObservers(std::string("bar"));
		lastNotifiedBar = bar;
	}
	
	if (lastNotifiedBeat != beat) {
		
		notifyObservers(std::string("beat"));
		lastNotifiedBeat = beat;
	}

	if (lastNotifiedClock != clock) {
		
		notifyObservers(std::string("clock"));
		lastNotifiedClock = clock;
	}
}

void Sequencer::goToPreviousStep()
{
	auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
	auto noteValue = timingCorrectScreen->getNoteValue();

	auto stepSize = TICK_VALUES[noteValue];
	auto pos = getTickPosition();
	auto stepCount = static_cast<int>(ceil(getActiveSequence()->getLastTick() / stepSize)) + 1;
    std::vector<int> stepGrid(stepCount);

	for (int i = 0; i < stepGrid.size(); i++)
		stepGrid[i] = i * stepSize;

	auto prevStepIndex = 0;

	for (auto l : stepGrid)
	{
		if (pos <= l)
			break;
		
		prevStepIndex++;
	}

	prevStepIndex--;

    if (prevStepIndex < 0) prevStepIndex = 0;

    move(prevStepIndex * stepSize);
}

void Sequencer::goToNextStep()
{
	auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
	auto noteValue = timingCorrectScreen->getNoteValue();

	auto stepSize = TICK_VALUES[noteValue];
	auto pos = getTickPosition();

    std::vector<int> stepGrid(ceil(getActiveSequence()->getLastTick() / stepSize));

	for (int i = 0; i < stepGrid.size(); i++)
		stepGrid[i] = i * stepSize;

	auto nextStepIndex = -1;

	for (auto l : stepGrid)
	{
		if (pos < l)
			break;

		nextStepIndex++;
	}

	nextStepIndex++;

    if (nextStepIndex >= stepGrid.size())
        nextStepIndex = stepGrid.size() - 1;

    move(nextStepIndex * stepSize);
}

void Sequencer::tap()
{
	if (isPlaying())
		return;

	auto now = moduru::System::currentTimeMillis();

	if (now - lastTap > 2000)
	{
		taps = { 0, 0, 0, 0 };
		tapIndex = 0;
	}

	lastTap = now;

	taps[tapIndex] = lastTap;
	
	uint64_t accum = 0;
	
	int index = tapIndex;

	tapIndex++;

	if (tapIndex > 3)
		tapIndex = 0;

	int usedTapsCounter = 0;

	for (int i = 0; i < 3; i++)
	{
		auto l0 = taps[index];

		index--;

		if (index < 0)
			index = 3;

		auto l1 = taps[index];
		
		if (l0 == 0 || l1 == 0)
			break;

		accum += l0 - l1;
		usedTapsCounter++;
	}

	if (accum == 0)
		return;

	auto newTempo = 60000.0 / (accum / usedTapsCounter);
	newTempo = floor(newTempo * 10) / 10;
	setTempo(newTempo);
}

int Sequencer::getResolution()
{
    return 96;
}

void Sequencer::move(int tick)
{
	auto oldTick = getTickPosition();
	position = tick;
	playStartTick = tick;

	auto s = isPlaying() ? getCurrentlyPlayingSequence() : getActiveSequence();

	if (!isPlaying() && songMode)
		s = sequences[getSongSequenceIndex()];

	s->resetTrackEventIndices(position);

	if (secondSequenceEnabled)
	{
		auto secondSequenceScreen = mpc.screens->get<SecondSeqScreen>("second-seq");
		sequences[secondSequenceScreen->sq]->resetTrackEventIndices(position);
	}

	notifyTimeDisplay();
    notifyObservers(std::string("timesignature"));
    notifyObservers(std::string("tempo"));
}

int Sequencer::getTickPosition()
{
    if (isPlaying())
        return mpc.getAudioMidiServices()->getFrameSequencer()->getTickPosition();
 
    return position;
}

std::shared_ptr<Sequence> Sequencer::getCurrentlyPlayingSequence()
{
    auto seqIndex = getCurrentlyPlayingSequenceIndex();

    if (seqIndex == -1)
    {
        return {};
    }

	return sequences[seqIndex];
}

void Sequencer::setActiveTrackIndex(int i)
{
    activeTrackIndex = i;
	notifyObservers(std::string("active-track-index"));
}

int Sequencer::getCurrentlyPlayingSequenceIndex()
{
	if (songMode)
    {
        auto songScreen = mpc.screens->get<SongScreen>("song");
        auto song = songs[songScreen->getActiveSongIndex()];
        if (!song->isUsed())
        {
            return -1;
        }
        auto songSeqIndex = songMode ? song->getStep(songScreen->getOffset() + 1).lock()->getSequence() : -1;
        return songSeqIndex;
    }

	return currentlyPlayingSequenceIndex;
}

void Sequencer::setCurrentlyPlayingSequenceIndex(int i)
{
	currentlyPlayingSequenceIndex = i;
    setActiveSequenceIndex(i);
}

int Sequencer::getNextSq()
{
    return nextSq;
}

int Sequencer::getFirstUsedSeqDown(int from, bool unused)
{
	for (int i = from; i >= 0; i--)
	{
		auto candidate = unused ? !sequences[i]->isUsed() : sequences[i]->isUsed();

		if (candidate)
			return i;
	}

	return -1;
}

int Sequencer::getFirstUsedSeqUp(int from, bool unused)
{
	for (int i = from; i < 99; i++)
	{
		auto candidate = unused ? !sequences[i]->isUsed() : sequences[i]->isUsed();

		if (candidate)
			return i;
	}

	return -1;
}

void Sequencer::setNextSq(int i)
{
	if (i < -1) i = -1;
	if (i > 98) i = 98;

	auto firstNotification = nextSq == -1;
	
	auto up = i > nextSq;
	
	if (firstNotification)
		up = i > currentlyPlayingSequenceIndex;

	auto candidate = up ? getFirstUsedSeqUp(i) : getFirstUsedSeqDown(i);

	if (up && candidate == -1)
		return;

	nextSq = candidate;

	if (nextSq == -1)
		notifyObservers(std::string("nextsqoff"));
	else if (firstNotification)
		notifyObservers(std::string("nextsq"));
	else
		notifyObservers(std::string("nextsqvalue"));
}

void Sequencer::setNextSqPad(int i)
{
	if (!sequences[i]->isUsed())
	{
		nextSq = -1;
		notifyObservers(std::string("nextsqoff"));
		return;
	}

	auto firstNotification = nextSq == -1;
	
	nextSq = i;
	
	if (firstNotification)
		notifyObservers(std::string("nextsq"));
	else
		notifyObservers(std::string("nextsqvalue"));
}

std::shared_ptr<Song> Sequencer::getSong(int i)
{
    return songs[i];
}

bool Sequencer::isSongModeEnabled()
{
   return songMode;
}

void Sequencer::setSongModeEnabled(bool b)
{
    songMode = b;
}

int Sequencer::getSongSequenceIndex()
{
	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto song = songs[songScreen->getActiveSongIndex()];
	auto step = songScreen->getOffset() + 1;

	if (step > song->getStepCount() - 1)
	{
		step = song->getStepCount() - 1;
	}
	return song->getStep(step).lock()->getSequence();
}

bool Sequencer::isSecondSequenceEnabled()
{
    return secondSequenceEnabled;
}

void Sequencer::setSecondSequenceEnabled(bool b)
{
    secondSequenceEnabled = b;
}

void Sequencer::flushTrackNoteCache()
{
	for (auto& t : getCurrentlyPlayingSequence()->getTracks())
		t->flushNoteCache();
}

void Sequencer::storeActiveSequenceInUndoPlaceHolder()
{
	auto copy = copySequence(sequences[activeSequenceIndex]);
	undoPlaceHolder.swap(copy);

	undoSeqAvailable = true;
}

bool Sequencer::isOverDubbing()
{
    return overdubbing;
}

int Sequencer::getPlayStartTick()
{
    return playStartTick;
}

void Sequencer::notify(std::string s)
{
	
	notifyObservers(s);
}

void Sequencer::setRecording(bool b)
{
    recording = b;
}

void Sequencer::setOverdubbing(bool b)
{
    overdubbing = b;
}

void Sequencer::playMetronomeTrack()
{
	if (isPlaying())
    {
		return;
	}

	metronomeOnly = true;
	metronomeSeq = std::make_unique<Sequence>(mpc);
	auto s = getActiveSequence();
	metronomeSeq->init(8);
	metronomeSeq->setTimeSignature(0, 3, s->getNumerator(getCurrentBarIndex()), s->getDenominator(getCurrentBarIndex()));
	metronomeSeq->setInitialTempo(getTempo());
	playStartTick = 0;
    mpc.getAudioMidiServices()->getFrameSequencer()->startMetronome();
}

void Sequencer::stopMetronomeTrack()
{
	if (!metronomeOnly)
    {
        return;
    }
	
    metronomeOnly = false;
	mpc.getAudioMidiServices()->getFrameSequencer()->stop();
}

std::shared_ptr<Sequence> Sequencer::createSeqInPlaceHolder()
{
	placeHolder = std::make_shared<Sequence>(mpc);
	return placeHolder;
}

void Sequencer::clearPlaceHolder()
{
	placeHolder.reset();
}

void Sequencer::movePlaceHolderTo(int destIndex)
{
	sequences[destIndex].swap(placeHolder);
	sequences[destIndex]->resetTrackEventIndices(position);
	clearPlaceHolder();
}

std::shared_ptr<Sequence> Sequencer::getPlaceHolder()
{
	return placeHolder;
}

int Sequencer::getPlayedStepRepetitions()
{
	return playedStepRepetitions;
}

void Sequencer::setEndOfSong(bool b)
{
	endOfSong = b;
}

void Sequencer::incrementPlayedStepRepetitions()
{
	playedStepRepetitions++;
}

void Sequencer::resetPlayedStepRepetitions()
{
	playedStepRepetitions = 0;
}
