#include "Sequencer.hpp"

#include <Mpc.hpp>

#include <audiomidi/AudioMidiServices.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <hardware/HwPad.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SongScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/CountMetronomeScreen.hpp>
#include <lcdgui/screens/UserScreen.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/FrameSeq.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/TimeSignature.hpp>
#include <sequencer/Song.hpp>
#include <sequencer/Step.hpp>

#include <sampler/Sampler.hpp>

// ctoot
#include <audio/server/NonRealTimeAudioServer.hpp>

// moduru
#include <System.hpp>
#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

using namespace mpc::sequencer;
using namespace std;

Sequencer::Sequencer(mpc::Mpc& mpc)
	: mpc(mpc)
{
}

void Sequencer::init()
{
	TICK_VALUES = vector<int>{ 1, 48, 32, 24, 16, 12, 8 };
	repeats = 0;
	sequences = vector<shared_ptr<Sequence>>(99);
	taps = make_unique<moduru::io::CircularIntBuffer>(4, true, true);
	reposition = -1;
	nextsq = -1;
	previousTempo = 0.0;
	
	auto userScreen = dynamic_pointer_cast<UserScreen>(mpc.screens->getScreenComponent("user"));
	defaultSequenceName = moduru::lang::StrUtil::trim(userScreen->sequenceName);
	
	for (int i = 0; i < 64; i++)
	{
		string name = "Track-";
		name = name.append(moduru::lang::StrUtil::padLeft(to_string(i + 1), "0", 2));
		defaultTrackNames.push_back(name);
	}

	activeTrackIndex = 0;
	songs = vector<shared_ptr<Song>>(20);
	defaultDeviceNames = vector<string>(33);
	
	for (int i = 0; i < 33; i++)
	{
		defaultDeviceNames[i] = userScreen->getDeviceName(i);
	}

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
	{
		songs[i] = make_shared<Song>(this);
		songs[i]->setName("Song" + moduru::lang::StrUtil::padLeft(to_string(i + 1), "0", 2));
	}
}

int Sequencer::getActiveSequenceIndex()
{
	return activeSequenceIndex;
}

weak_ptr<Track> Sequencer::getActiveTrack()
{
	if (!getActiveSequence().lock())
	{
		return {};
	}

	return getActiveSequence().lock()->getTrack(activeTrackIndex);
}

void Sequencer::playToTick(int targetTick)
{
	auto seqIndex = songMode ? getSongSequenceIndex() : currentlyPlayingSequenceIndex;
	auto tc = metronomeOnly ? metronomeSeq.get() : sequences[seqIndex].get();
	for (auto& trk : tc->getTracks()) {
		auto lTrk = trk.lock();
		while (lTrk->getNextTick() <= targetTick) {
			lTrk->playNext();
		}
	}
	for (auto& trk : tc->getMetaTracks()) {
		auto lTrk = trk.lock();
		while (lTrk->getNextTick() <= targetTick) {
			lTrk->playNext();
		}
	}
}

vector<int> Sequencer::getTickValues() {
	return TICK_VALUES;
}

int Sequencer::repeats;
bool Sequencer::endOfSong = false;

void Sequencer::setTempo(const double newTempo)
{
	if (newTempo < 30.0 || newTempo > 300.0)
	{
		return;
	}

	auto s = getActiveSequence().lock();

	if (s && s->isUsed() && tempoSourceSequenceEnabled)
	{
		auto tce = getCurrentTempoChangeEvent().lock();
	
		if (tce->getTick() == 0)
		{
			s->setInitialTempo(newTempo);
		}
		else
		{
			auto initialTempo = s->getInitialTempo();
			auto ratio = newTempo / initialTempo;
			tce->setRatio((int)(ratio * 1000.0));
		}
	}
	else
	{
		tempo = newTempo;
	}
	
	notifyObservers(string("tempo"));
}

double Sequencer::getTempo()
{
	if (!isPlaying() && !getActiveSequence().lock()->isUsed())
	{
		return tempo;
	}

	if (tempoSourceSequenceEnabled)
	{
		auto tce = getCurrentTempoChangeEvent().lock();
		if (tce)
		{
			return tce->getTempo();
		}
		return getActiveSequence().lock()->getInitialTempo();
	}
	return tempo;
}

weak_ptr<TempoChangeEvent> Sequencer::getCurrentTempoChangeEvent()
{
	auto index = -1;
	auto s = getActiveSequence().lock();
	
	for (auto& tce : s->getTempoChangeEvents())
	{
		auto lTce = tce.lock();
		if (getTickPosition() >= lTce->getTick())
		{
			index++;
		}
		else
		{
			break;
		}
	}
	
	if (index == -1)
	{
		index++;
	}
	return s->getTempoChangeEvents()[index];
}

bool Sequencer::isTempoSourceSequenceEnabled()
{
    return tempoSourceSequenceEnabled;
}

void Sequencer::setTempoSourceSequence(bool b)
{
	tempoSourceSequenceEnabled = b;
	
	notifyObservers(string("temposource"));
	
	notifyObservers(string("tempo"));
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
    soloEnabled = b;
    
    notifyObservers(string("soloenabled"));
}

weak_ptr<Sequence> Sequencer::getSequence(int i)
{
    return sequences[i];
}

string Sequencer::getDefaultSongName()
{
    return defaultSongName;
}

void Sequencer::setDefaultSongName(string s)
{
    defaultSongName = s;
}

string Sequencer::getDefaultSequenceName()
{
	return defaultSequenceName;
}

void Sequencer::setDefaultSequenceName(string s)
{
    defaultSequenceName = s;
}

void Sequencer::setActiveSequenceIndex(int i)
{
	if (i < 0 || i > 98)
	{
		return;
	}

	activeSequenceIndex = i;
	
	if (!isPlaying())
	{
		position = 0;
		
		notifyObservers(string("now"));
	}
	
	
	notifyObservers(string("seqnumbername"));
	
	notifyObservers(string("timesignature"));
	
	notifyObservers(string("numberofbars"));
	
	notifyObservers(string("tempo"));
	notifyTrack();
}

string Sequencer::getDefaultDeviceName(int i)
{
    return defaultDeviceNames[i];
}

void Sequencer::setDefaultDeviceName(int i, string s)
{
	defaultDeviceNames[i] = s;
}

bool Sequencer::isCountEnabled()
{
    return countEnabled;
}

void Sequencer::setCountEnabled(bool b)
{
    countEnabled = b;
    
    notifyObservers(string("count"));
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
    
    notifyObservers(string("recordingmode"));
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
	auto ams = mpc.getAudioMidiServices().lock();
	auto frameSequencer = ams->getFrameSequencer().lock();
	auto server = ams->getAudioServer();
	
	if (!server->isRunning() || !frameSequencer) {
		return false;
	}

	return ams->getFrameSequencer().lock()->isRunning();
}

void Sequencer::play(bool fromStart)
{
	if (isPlaying())
	{
		return;
	}

    endOfSong = false;
    repeats = 0;
	auto songScreen = dynamic_pointer_cast<SongScreen>(mpc.screens->getScreenComponent("song"));
	auto currentSong = songs[songScreen->getSelectedSongIndex()];
    
	Step* currentStep = nullptr;
	if (songMode)
	{
		if (!currentSong->isUsed())
		{
			return;
		}

		if (fromStart)
		{
			songScreen->setOffset(-1);
		}
		
		if (songScreen->getOffset() + 1 > currentSong->getStepAmount() - 1)
		{
			return;
		}
		
		int step = songScreen->getOffset() + 1;
		
		if (step > currentSong->getStepAmount())
		{
			step = currentSong->getStepAmount() - 1;
		}
		
		currentStep = currentSong->getStep(step);
	}

	move(position);
    
	currentlyPlayingSequenceIndex = activeSequenceIndex;

	auto countMetronomeScreen = dynamic_pointer_cast<CountMetronomeScreen>(mpc.screens->getScreenComponent("count-metronome"));
	auto countInMode = countMetronomeScreen->getCountInMode();

    if (!countEnabled || countInMode == 0 || (countInMode == 1 && recording == false))
	{
		if (fromStart)
		{
			move(0);
		}
    }
	
	auto s = getActiveSequence().lock();
	
	if (countEnabled && !songMode)
	{
		if (countInMode == 2 || (countInMode == 1 && recording == true))
		{
			move(s->getLoopStart());
			startCountingIn();
		}
	}

	auto hw = mpc.getHardware().lock();
	
	if (!songMode)
	{
		if (!s->isUsed())
		{
			return;
		}

		s->initLoop();

		if (recording || overdubbing) {
			auto copy = copySequence(s);
			undoPlaceHolder.swap(copy);
			lastRecordingActive = true;
			recordStartTick = getTickPosition();
			hw->getLed("undoseq").lock()->light(lastRecordingActive);
		}

	}

	hw->getLed("play").lock()->light(true);

	auto ams = mpc.getAudioMidiServices().lock();

	if (ams->isBouncePrepared())
	{
		ams->startBouncing();
	}
	else
	{
		int rate = ams->getAudioServer()->getSampleRate();
		ams->getFrameSequencer().lock()->start(rate);
	}

	
    notifyObservers(string("play"));
}

void Sequencer::undoSeq()
{
	if (isPlaying()) {
		return;
	}
	
	if (!undoPlaceHolder) {
		return;
	}

	auto s = copySequence(undoPlaceHolder);
	auto copy = copySequence(sequences[activeSequenceIndex]);	
	undoPlaceHolder.swap(copy);

	sequences[activeSequenceIndex].swap(s);
	
	sequences[activeSequenceIndex]->resetTrackEventIndices(position);

	lastRecordingActive = !lastRecordingActive;
	auto hw = mpc.getHardware().lock();
	hw->getLed("undoseq").lock()->light(lastRecordingActive);

	setActiveSequenceIndex(getActiveSequenceIndex()); // Shortcut to notifying SequencerObserver
}

void Sequencer::clearUndoSeq()
{
	if (isPlaying()) {
		return;
	}
	
	undoPlaceHolder.reset();

    lastRecordingActive = false;
	auto hw = mpc.getHardware().lock();
	hw->getLed("undoseq").lock()->light(false);
}

void Sequencer::playFromStart()
{
	if (isPlaying()) {
		return;
	}
	play(true);
}

void Sequencer::play()
{
	if (isPlaying()) {
		return;
	}
	play(false);
}

void Sequencer::rec()
{
	if (isPlaying()) {
		return;
	}

	recording = true;
	
	play(false);
}

void Sequencer::recFromStart()
{
	if (isPlaying()) {
		return;
	}
	recording = true;
	play(true);
}

void Sequencer::overdub()
{
	if (isPlaying()) {
		return;
	}
	overdubbing = true;
	play(false);
}

void Sequencer::switchRecordToOverDub()
{
	if (!isRecording()) {
		return;
	}
	recording = false;
	overdubbing = true;
	auto hw = mpc.getHardware().lock();
	hw->getLed("overdub").lock()->light(true);
	hw->getLed("rec").lock()->light(false);
}

void Sequencer::overdubFromStart()
{
	if (isPlaying()) {
		return;
	}
	overdubbing = true;
	play(true);
}

void Sequencer::stop(){
	stop(-1);
}

void Sequencer::stop(int tick)
{
	auto ams = mpc.getAudioMidiServices().lock();
	bool bouncing = ams->isBouncing();

	if (!isPlaying() && !bouncing) {
		if (position != 0)
            setBar(0); // real 2kxl doesn't do this
        return;
    }
	
	lastNotifiedBar = -1;
	lastNotifiedBeat = -1;
	lastNotifiedClock = -1;
    //mpc.getEventHandler()->handle(MidiClockEvent(ctoot::midi::core::ShortMessage::STOP), Track(999));
	auto s1 = getActiveSequence().lock();
	auto s2 = getCurrentlyPlayingSequence().lock();
	auto pos = getTickPosition();
	
	if (pos > s1->getLastTick()) {
		pos = s1->getLastTick();
	}

	int frameOffset = tick == -1 ? 0 : ams->getFrameSequencer().lock()->getEventFrameOffset(tick);
	ams->getFrameSequencer().lock()->stop();
	if (recording || overdubbing) {
		auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(mpc.screens->getScreenComponent("timing-correct"));
		auto noteValue = timingCorrectScreen->getNoteValue();
		s2->getTrack(activeTrackIndex).lock()->correctTimeRange(0, s2->getLastTick(), TICK_VALUES[noteValue]);
	}

    auto notifynextsq = false;
	if (nextsq != -1) {
		notifynextsq = true;
		nextsq = -1;

		// This is called from the audio thread, should be called from the UI thread instead.
		// stop() is called by FrameSeq.
		mpc.getLayeredScreen().lock()->setFocus("sq");
	}
    recording = false;
    overdubbing = false;
    
    move(pos);
	if (!bouncing) {
		mpc.getSampler().lock()->stopAllVoices(frameOffset);
	}
	for (int i = 0; i < 16; i++) {
		mpc.getHardware().lock()->getPad(i).lock()->notifyObservers(255);
	}
    if (notifynextsq) {
        
        notifyObservers(string("nextsqoff"));
    }
    
	notifyTimeDisplay();
	
	auto songScreen = dynamic_pointer_cast<SongScreen>(mpc.screens->getScreenComponent("song"));

    if (endOfSong)
	{
		songScreen->setOffset(songScreen->getOffset() + 1);
    }
	
	if (bouncing)
	{
		ams->stopBouncing();
	}
	
	auto hw = mpc.getHardware().lock();

	hw->getLed("overdub").lock()->light(false);
	hw->getLed("play").lock()->light(false);
	hw->getLed("rec").lock()->light(false);

    
    notifyObservers(string("stop"));
}

bool Sequencer::isCountingIn()
{
    return countingIn;
}

void Sequencer::setCountingIn(bool b)
{
    countingIn = b;
}

void Sequencer::startCountingIn()
{
    countingIn = true;
}

void Sequencer::notifyTrack()
{
    
    notifyObservers(string("tracknumbername"));
    
    notifyObservers(string("trackon"));
    
    notifyObservers(string("programchange"));
    
    notifyObservers(string("velocityratio"));
    
    notifyObservers(string("bus"));
    
    notifyObservers(string("device"));
    
    notifyObservers(string("devicename"));
}

void Sequencer::setSequence(int i, shared_ptr<Sequence> s)
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
	auto sequence = make_shared<Sequence>(mpc, defaultTrackNames);
	sequences[i].swap(sequence);
	sequences[i]->resetTrackEventIndices(position);
	string res = defaultSequenceName;
	res.append(moduru::lang::StrUtil::padLeft(to_string(i + 1), "0", 2));
	sequences[i]->setName(res);
}

void Sequencer::copySequence(int source, int destination)
{
	auto copy = copySequence(sequences[source]);
	sequences[destination].swap(copy);
	sequences[destination]->resetTrackEventIndices(position);
	sequences[destination]->initLoop();
}

shared_ptr<Sequence> Sequencer::copySequence(weak_ptr<Sequence> src)
{
	auto source = src.lock();
	auto copy = make_shared<Sequence>(mpc, defaultTrackNames);
	copy->init(source->getLastBarIndex());
	copySequenceParameters(source, copy);
	
	for (int i = 0; i < 64; i++) {
		copyTrack(source->getTrack(i), copy->getTrack(i));
	}

	for (int i = 0; i < source->getMetaTracks().size(); i++) {
		copy->getMetaTracks()[i].lock()->removeEvents();
		copyTrack(source->getMetaTracks()[i], copy->getMetaTracks()[i]);
	}

	for (auto& tempo : copy->getTempoChangeEvents()) {
		tempo.lock()->setParent(copy.get());
	}

	return copy;
}

void Sequencer::copySequenceParameters(weak_ptr<Sequence> src, weak_ptr<Sequence> dst)
{
	auto source = src.lock();
	auto dest = dst.lock();
	dest->setName(source->getName());
	copyTempoChangeEvents(src, dst);
	dest->setLoopEnabled(source->isLoopEnabled());
	dest->setUsed(source->isUsed());
	dest->setDeviceNames(source->getDeviceNames());
	dest->setInitialTempo(source->getInitialTempo());
	dest->setBarLengths(*source->getBarLengths());
	dest->setNumeratorsAndDenominators(*source->getNumerators(), *source->getDenominators());
	dest->setLoopStart(source->getLoopStart());
	dest->setLoopEnd(source->getLoopEnd());
}

void Sequencer::copyTempoChangeEvents(weak_ptr<Sequence> src, weak_ptr<Sequence> dst) {
	auto srcList = src.lock()->getTempoChangeEvents();
	
}

void Sequencer::copyTrack(int sourceTrackIndex, int destinationTrackIndex, int sourceSequenceIndex, int destinationSequenceIndex)
{
	auto src = sequences[sourceSequenceIndex]->getTrack(sourceTrackIndex).lock();
	auto dest = sequences[destinationSequenceIndex]->purgeTrack(destinationTrackIndex).lock();
	copyTrack(src, dest);
}

void Sequencer::copyTrack(weak_ptr<Track> src, weak_ptr<Track> dest)
{
	auto lSrc = src.lock();
	auto lDest = dest.lock();
	
	lDest->setTrackIndex(lSrc->getTrackIndex());
	
	for (auto& e : lSrc->getEvents())
	{
		lDest->cloneEvent(e);
	}

	copyTrackParameters(src, dest);
}

void Sequencer::copyTrackParameters(weak_ptr<Track> source, weak_ptr<Track> dest)
{
	auto lSrc = source.lock();
	auto lDest = dest.lock();
	lDest->setUsed(lSrc->isUsed());
    lDest->setOn(lSrc->isOn());
    lDest->setDeviceNumber(lSrc->getDevice());
    lDest->setBusNumber(lSrc->getBus());
    lDest->setVelocityRatio(lSrc->getVelocityRatio());
    lDest->setProgramChange(lSrc->getProgramChange());
    lDest->setName(lSrc->getName());
}

string Sequencer::getDefaultTrackName(int i)
{
	return defaultTrackNames[i];
}

vector<string> Sequencer::getDefaultTrackNames()
{
	return defaultTrackNames;
}

void Sequencer::setDefaultTrackName(string s, int i)
{
	defaultTrackNames[i] = s;
}

int Sequencer::getCurrentBarIndex()
{
	auto s = isPlaying() ? getCurrentlyPlayingSequence().lock() : getActiveSequence().lock();
	auto pos = getTickPosition();
	if (pos == s->getLastTick()) return s->getLastBarIndex() + 1;
	auto index = pos;
	if (isPlaying() && !countingIn) index = getTickPosition();
	if (index == 0) return 0;

	auto barLengths = s->getBarLengths();
	auto barCounter = 0;

	int tickCounter = 0;
	for (int i = 0; i < 999; i++) {
		if (i > s->getLastBarIndex()) i = 0;
		tickCounter += (*barLengths)[i];
		if (tickCounter > index) {
			barCounter = i;
			break;
		}
	}
	return barCounter;
}

int Sequencer::getCurrentBeatIndex()
{
	auto s = isPlaying() ? getCurrentlyPlayingSequence().lock() : getActiveSequence().lock();
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
	for (auto l : *s->getBarLengths()) {
		if (barCounter == getCurrentBarIndex()) break;
		barStartPos += l;
		barCounter++;
	}
	auto beatCounter = (int)(floor((index - barStartPos) / denTicks));
	return beatCounter;
}

int Sequencer::getCurrentClockNumber()
{
	auto s = isPlaying() ? getCurrentlyPlayingSequence().lock() : getActiveSequence().lock();

	int pos = getTickPosition();
	
	if (pos == s->getLastTick())
	{
		return 0;
	}
	
	int index = pos;
	
	if (isPlaying() && !countingIn)
	{
		if (index > s->getLastTick())
		{
			index %= s->getLastTick();
		}
	}

	auto ts = s->getTimeSignature();
	auto den = ts.getDenominator();
	auto denTicks = 96 * (4.0 / den);

	if (index == 0)
	{
		return index;
	}

	auto barCounter = 0;
	int clock = index;

	for (auto l : *s->getBarLengths())
	{
		if (barCounter == getCurrentBarIndex())
		{
			break;
		}
		clock -= l;
		barCounter++;
	}
	
	for (int i = 0; i < getCurrentBeatIndex(); i++)
	{
		clock -= denTicks;
	}

	return clock;
}

void Sequencer::setBar(int i)
{
	if (isPlaying())
	{
		return;
	}

	if (i < 0)
	{
		move(0);
		return;
	}

	auto s = getActiveSequence().lock();
	
	if (i > s->getLastBarIndex() + 1)
	{
		return;
	}

	if (s->getLastBarIndex() == 998 && i > 998)
	{
		return;
	}
	auto ts = s->getTimeSignature();
	auto den = ts.getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	
	if (i != s->getLastBarIndex() + 1)
	{
		ts.setNumerator(s->getNumerator(i));
		ts.setDenominator(s->getDenominator(i));
	}

	auto barLengths = s->getBarLengths();
	auto currentClock = getCurrentClockNumber();
	auto currentBeat = getCurrentBeatIndex();
	int pos = 0;
	auto barCounter = 0;
	
	for (auto l : *barLengths)
	{
		if (barCounter == i)
		{
			break;
		}
		pos += l;
		barCounter++;
	}

	pos += currentBeat * denTicks;
	pos += currentClock;
	
	if (pos > s->getLastTick())
	{
		pos = s->getLastTick();
	}

	move(pos);
	
	notifyObservers(string("timesignature"));
	setBeat(0);
	setClock(0);
}

void Sequencer::setBeat(int i)
{
	if (i < 0 || isPlaying())
	{
		return;
	}

	auto s = getActiveSequence().lock();
	auto pos = getTickPosition();
	
	if (pos == s->getLastTick())
	{
		return;
	}
	
	auto ts = s->getTimeSignature();
	auto difference = i - getCurrentBeatIndex();
	auto num = ts.getNumerator();
	
	if (i >= num)
	{
		return;
	}

	auto den = ts.getDenominator();
	auto denTicks = 96 * (4.0 / den);
	pos += difference * denTicks;
	move(pos);
}

void Sequencer::setClock(int i)
{
	if (i < 0 || isPlaying())
	{
		return;
	}

	auto s = getActiveSequence().lock();
	int pos = getTickPosition();
	
	if (pos == s->getLastTick())
	{
		return;
	}
	
	getCurrentClockNumber();
	int difference = i - getCurrentClockNumber();
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = 96 * (4.0 / den);

	if (i > denTicks - 1)
	{
		return;
	}

	if (pos + difference > s->getLastTick())
	{
		return;
	}

	pos += difference;
	move(pos);
}

int Sequencer::getLoopEnd()
{
    return getActiveSequence().lock()->getLoopEnd();
}

weak_ptr<Sequence> Sequencer::getActiveSequence()
{
	auto songScreen = dynamic_pointer_cast<SongScreen>(mpc.screens->getScreenComponent("song"));

	if (songMode && songs[songScreen->getSelectedSongIndex()]->getStepAmount() != 0)
	{
		return sequences[getSongSequenceIndex() >= 0 ? getSongSequenceIndex() : activeSequenceIndex];
	}

	return sequences[activeSequenceIndex];
}

int Sequencer::getUsedSequenceCount()
{
    return getUsedSequences().size();
}

vector<weak_ptr<Sequence>> Sequencer::getUsedSequences()
{
	vector<weak_ptr<Sequence>> usedSeqs;

	for (auto s : sequences)
	{
		if (s->isUsed())
		{
			usedSeqs.push_back(s);
		}
	}

    return usedSeqs;
}

vector<int> Sequencer::getUsedSequenceIndexes()
{
	vector<int> usedSeqs;

	for (int i = 0; i < 99; i++)
	{
        auto s = sequences[i];
	
		if (s->isUsed())
		{
			usedSeqs.push_back(i);
		}
    }
	return usedSeqs;
}

void Sequencer::goToPreviousEvent()
{
	auto t = getActiveSequence().lock()->getTrack(getActiveTrackIndex()).lock();

	if (t->getEventIndex() == 0)
	{
		setBar(0);
		return;
	}

	if (t->getEventIndex() + 1 >= t->getEvents().size() && t->getEvent((int)(t->getEvents().size()) - 1).lock()->getTick() < position)
	{
		t->setEventIndex((int)(t->getEvents().size()) - 1);
		move(t->getEvent(t->getEventIndex()).lock()->getTick());
		return;
	}

	shared_ptr<Event> event;
	shared_ptr<Event> prev;
	
	while (t->getEventIndex() > 0)
	{
		event = t->getEvent(t->getEventIndex()).lock();
		prev = t->getEvent(t->getEventIndex() - 1).lock();
	
		if (prev->getTick() == event->getTick())
		{
			t->setEventIndex(t->getEventIndex() - 1);
		} else
		{
			break;
		}
	}

	t->setEventIndex(t->getEventIndex() - 1);
	
	while (t->getEventIndex() > 0)
	{
		event = t->getEvent(t->getEventIndex()).lock();
		prev = t->getEvent(t->getEventIndex() - 1).lock();
	
		if (prev->getTick() != event->getTick())
		{
			break;
		}

		t->setEventIndex(t->getEventIndex() - 1);
	}

	move(t->getEvents()[t->getEventIndex()].lock()->getTick());
}

void Sequencer::goToNextEvent()
{
	auto s = getActiveSequence().lock();
	auto t = s->getTrack(getActiveTrackIndex()).lock();

	if (t->getEvents().size() == 0)
	{
		if (position != s->getLastTick())
		{
			move(s->getLastTick());
		}
		return;
	}

	const int eventCount = t->getEvents().size();
	
	if (position == s->getLastTick())
	{
		return;
	}

	if (t->getEventIndex() >= eventCount - 1 && position >= t->getEvent(eventCount - 1).lock()->getTick())
	{
		move(s->getLastTick());
		return;
	}
	
	if (t->getEvent(t->getEventIndex()).lock()->getTick() > position)
	{
		move(t->getEvent(t->getEventIndex()).lock()->getTick());
		return;
	}
	
	shared_ptr<Event> event;
	shared_ptr<Event> next;
	
	if (t->getEvent(t->getEventIndex()).lock()->getTick() == position)
	{
		while (t->getEventIndex() < eventCount - 2)
		{
			event = t->getEvent(t->getEventIndex()).lock();
			next = t->getEvent(t->getEventIndex() + 1).lock();
		
			if (next->getTick() != event->getTick())
			{
				break;
			}

			t->setEventIndex(t->getEventIndex() + 1);
		}
	}

	t->setEventIndex(t->getEventIndex() + 1);
	
	while (t->getEventIndex() < eventCount - 2)
	{
		event = t->getEvent(t->getEventIndex()).lock();
		next = t->getEvent(t->getEventIndex() + 1).lock();

		if (next->getTick() != event->getTick()) {
			break;
		}

		t->setEventIndex(t->getEventIndex() + 1);
	}

	move(t->getEvent(t->getEventIndex()).lock()->getTick());
}

void Sequencer::notifyTimeDisplay()
{
    
	notifyObservers(string("bar"));
	notifyObservers(string("beat"));
	notifyObservers(string("clock"));
}

void Sequencer::notifyTimeDisplayRealtime()
{
	int bar = getCurrentBarIndex();
	int beat = getCurrentBeatIndex();
	int clock = getCurrentClockNumber();
	if (lastNotifiedBar != bar) {
		
		notifyObservers(string("bar"));
		lastNotifiedBar = bar;
	}
	if (lastNotifiedBeat != beat) {
		
		notifyObservers(string("beat"));
		lastNotifiedBeat = beat;
	}
	if (lastNotifiedClock != clock) {
		
		notifyObservers(string("clock"));
		lastNotifiedClock = clock;
	}
}

void Sequencer::goToPreviousStep()
{
	auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(mpc.screens->getScreenComponent("timing-correct"));
	auto noteValue = timingCorrectScreen->getNoteValue();

	auto stepSize = TICK_VALUES[noteValue];
	auto pos = getTickPosition();
	auto stepAmt = static_cast<int>(ceil(getActiveSequence().lock()->getLastTick() / stepSize)) + 1;
	auto stepGrid = vector<int>(stepAmt);

	for (int i = 0; i < stepGrid.size(); i++) {
		stepGrid[i] = i * stepSize;
	}

	auto currentStep = 0;

	for (auto l : stepGrid) {
		if (pos <= l) break;
		currentStep++;
	}

	if (currentStep == 0) {
		return;
	}

	currentStep--;
	move(currentStep * stepSize);
}

void Sequencer::goToNextStep()
{
	auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(mpc.screens->getScreenComponent("timing-correct"));
	auto noteValue = timingCorrectScreen->getNoteValue();

	auto stepSize = TICK_VALUES[noteValue];
	auto pos = getTickPosition();

	auto stepGrid = vector<int>(ceil(getActiveSequence().lock()->getLastTick() / stepSize));

	for (int i = 0; i < stepGrid.size(); i++)
		stepGrid[i] = i * stepSize;

	auto currentStep = -1;
	for (auto l : stepGrid) {
		if (pos < l) {
			break;
		}
		currentStep++;
	}

	if (currentStep == stepGrid.size()) {
		return;
	}

	currentStep++;
	move(currentStep * stepSize);
}

void Sequencer::tap()
{
	if (isPlaying())
	{
		return;
	}

	auto nanoLong = moduru::System::nanoTime();

	if (nanoLong - lastTap > (2000 * 1000000))
	{
		taps = make_unique<moduru::io::CircularIntBuffer>(4, true, true);
	}

	lastTap = nanoLong;
	taps->write(vector<int>{ (int)nanoLong });
	int accum = 0;
	vector<long> tapsLong;
	
	while (taps->availableRead() > 0)
	{
		tapsLong.push_back(taps->read());
	}
	
	for (int i = 0; i < (int)(tapsLong.size()) - 1; i++)
	{
		int l0 = tapsLong[i];
		int l1 = tapsLong[i + 1];
		accum += l1 - l0;
	}

	if (accum == 0) return;

	auto tempo = (60000.0 * 1000000.0) / (accum / ((int)(tapsLong.size()) - 1));
	tempo = floor(tempo * 10) / 10;
	setTempo(tempo);
}

int Sequencer::getResolution()
{
    return 96;
}

void Sequencer::move(int tick)
{
	auto oldTick = getTickPosition();
	reposition = tick;
	position = tick;
	playStartTick = tick;
	
	auto s = isPlaying() ? getCurrentlyPlayingSequence().lock() : getActiveSequence().lock();

	if (!isPlaying() && songMode)
	{
		s = sequences[getSongSequenceIndex()];
	}

	s->resetTrackEventIndices(position);

	notifyTimeDisplay();

	if (getTempo() != previousTempo)
	{
		previousTempo = getTempo();	
		notifyObservers(string("tempo"));
	}
}

int Sequencer::getTickPosition()
{
    if (isPlaying()) {
        return mpc.getAudioMidiServices().lock()->getFrameSequencer().lock()->getTickPosition();
    }
    return position;
}

weak_ptr<Sequence> Sequencer::getCurrentlyPlayingSequence()
{
	return sequences[getCurrentlyPlayingSequenceIndex()];
}

void Sequencer::setActiveTrackIndex(int i)
{
    activeTrackIndex = i;
	notifyObservers(string("active-track-index"));
}

int Sequencer::getCurrentlyPlayingSequenceIndex()
{
	auto songScreen = dynamic_pointer_cast<SongScreen>(mpc.screens->getScreenComponent("song"));
	auto songSeqIndex = songMode ? songs[songScreen->getSelectedSongIndex()]->getStep(songScreen->getOffset() + 1)->getSequence() : -1;
	return songMode ? songSeqIndex : currentlyPlayingSequenceIndex;
}

void Sequencer::setCurrentlyPlayingSequenceIndex(int i) {
	currentlyPlayingSequenceIndex = i;
	activeSequenceIndex = i;
}

int Sequencer::getNextSq()
{
    return nextsq;
}

int Sequencer::getFirstUsedSeqDown(int from)
{
	auto result = -1;
	for (int i = from; i >= 0; i--)
	{
		if (sequences[i]->isUsed())
		{
			result = i;
			break;
		}
	}
	return result;
}

int Sequencer::getFirstUsedSeqUp(int from)
{
	auto result = -1;

	for (int i = from; i < 99; i++)
	{
		if (sequences[i]->isUsed())
		{
			result = i;
			break;
		}
	}
	return result;
}

void Sequencer::resetNextSq()
{
	nextsq = -1;
}

void Sequencer::setNextSq(int i)
{
	if (!isPlaying())
	{
		return;
	}

	auto firstNotification = nextsq == -1;
	
	auto up = i > nextsq;
	
	if (firstNotification)
	{
		up = i > currentlyPlayingSequenceIndex;
	}

	auto candidate = up ? getFirstUsedSeqUp(i) : getFirstUsedSeqDown(i);

	if (candidate == -1) {
		return;
	}

	nextsq = candidate;
	

	if (firstNotification)
	{
		notifyObservers(string("nextsq"));
	}
	else
	{
		notifyObservers(string("nextsqvalue"));
	}
}

void Sequencer::setNextSqPad(int i)
{
	if (!isPlaying())
	{
		return;
	}

	if (!sequences[i]->isUsed())
	{
		nextsq = -1;
		
		notifyObservers(string("nextsqoff"));
		return;
	}

	auto firstNotification = nextsq == -1;
	
	nextsq = i;
	
	if (firstNotification)
	{
		
		notifyObservers(string("nextsq"));
	}
	else {
		
		notifyObservers(string("nextsqvalue"));
	}
}

weak_ptr<Song> Sequencer::getSong(int i)
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
	auto songScreen = dynamic_pointer_cast<SongScreen>(mpc.screens->getScreenComponent("song"));
	auto song = songs[songScreen->getSelectedSongIndex()];
	auto step = songScreen->getOffset() + 1;

	if (step > song->getStepAmount() - 1)
	{
		step = song->getStepAmount() - 1;
	}
	return song->getStep(step)->getSequence();
}

bool Sequencer::isSecondSequenceEnabled()
{
    return secondSequenceEnabled;
}

void Sequencer::setSecondSequenceEnabled(bool b)
{
    secondSequenceEnabled = b;
}

int Sequencer::getSecondSequenceIndex()
{
    return secondSequenceIndex;
}

void Sequencer::setSecondSequenceIndex(int i)
{
    secondSequenceIndex = i;
}

void Sequencer::flushTrackNoteCache()
{
	for (auto& t : getCurrentlyPlayingSequence().lock()->getTracks()) {
		t.lock()->flushNoteCache();
	}
}

void Sequencer::storeActiveSequenceInUndoPlaceHolder()
{
	auto copy = copySequence(sequences[activeSequenceIndex]);
	undoPlaceHolder.swap(copy);

	lastRecordingActive = true;
	auto hw = mpc.getHardware().lock();
	hw->getLed("undoseq").lock()->light(lastRecordingActive);
}

bool Sequencer::isOverDubbing()
{
    return overdubbing;
}

int Sequencer::getPlayStartTick()
{
    return playStartTick;
}

void Sequencer::notify(string s)
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
	if (isPlaying() || metronomeOnly) {
		return;
	}

	metronomeOnly = true;
	metronomeSeq = make_unique<Sequence>(mpc, defaultTrackNames);
	auto s = getActiveSequence().lock();
	metronomeSeq->init(8);
	metronomeSeq->setTimeSignature(0, 3, s->getNumerator(getCurrentBarIndex()), s->getDenominator(getCurrentBarIndex()));
	metronomeSeq->setInitialTempo(getTempo());
	metronomeSeq->removeFirstMetronomeClick();
	auto lAms = mpc.getAudioMidiServices().lock();
	auto fs = lAms->getFrameSequencer().lock();
	playStartTick = 0;
	fs->startMetronome(lAms->getAudioServer()->getSampleRate());
}

void Sequencer::stopMetronomeTrack()
{
	if (!metronomeOnly) return;
	metronomeOnly = false;
	mpc.getAudioMidiServices().lock()->getFrameSequencer().lock()->stop();
}

weak_ptr<Sequence> Sequencer::createSeqInPlaceHolder() {
	placeHolder = make_shared<Sequence>(mpc, defaultTrackNames);
	return placeHolder;
}

void Sequencer::clearPlaceHolder() {
	placeHolder.reset();
}

void Sequencer::movePlaceHolderTo(int destIndex) {
	sequences[destIndex].swap(placeHolder);
	sequences[destIndex]->resetTrackEventIndices(position);
	clearPlaceHolder();
}

weak_ptr<Sequence> Sequencer::getPlaceHolder() {
	return placeHolder;
}
