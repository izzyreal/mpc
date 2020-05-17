#include "SequencerWindowGui.hpp"

#include <sequencer/Event.hpp>
#include <sequencer/TempoChangeEvent.hpp>

using namespace mpc::ui::sequencer::window;
using namespace std;

SequencerWindowGui::SequencerWindowGui() 
{

	inPlay = true;
	inRec = true;
	clickVolume = 100;
	accentVelo = 127;
	accentNote = 35;
	normalVelo = 64;
	normalNote = 35;
}

int SequencerWindowGui::getTrackNumber()
{
    return trackNumber;
}

void SequencerWindowGui::setTrackNumber(int i)
{
	if (i < 0 || i > 63) {
		return;
	}
    trackNumber = i;
    setChanged();
    notifyObservers(string("tracknumber"));
}

int SequencerWindowGui::getSq0()
{
    return sq0;
}

void SequencerWindowGui::setSq0(int i)
{
	if (i < 0 || i > 98) {
		return;
	}
	sq0 = i;
	setChanged();
	notifyObservers(string("sq0"));
}

int SequencerWindowGui::getSq1()
{
    return sq1;
}

void SequencerWindowGui::setSq1(int i)
{
	if (i < 0 || i > 98) {
		return;
	}
    sq1 = i;
    setChanged();
    notifyObservers(string("sq1"));
}

int SequencerWindowGui::getTr0()
{
    return tr0;
}

void SequencerWindowGui::setTr0(int i)
{
	if (i < 0 || i > 63) {
		return;
	}
	tr0 = i;
	setChanged();
	notifyObservers(string("tr0"));
}

int SequencerWindowGui::getTr1()
{
    return tr1;
}

void SequencerWindowGui::setTr1(int i)
{
	if (i < 0 || i > 63) {
		return;
	}
	tr1 = i;
	setChanged();
	notifyObservers(string("tr1"));
}

vector<weak_ptr<mpc::sequencer::TempoChangeEvent>> SequencerWindowGui::getVisibleTempoChanges()
{
    return visibleTempoChanges;
}

void SequencerWindowGui::setVisibleTempoChanges(vector<weak_ptr<mpc::sequencer::TempoChangeEvent>> vtc)
{
	visibleTempoChanges = vtc;
}

int SequencerWindowGui::getTempoChangeOffset()
{
    return tempoChangeOffset;
}

void SequencerWindowGui::setTempoChangeOffset(int i)
{
	if (i < 0) return;
	tempoChangeOffset = i;
	setChanged();
	notifyObservers(string("offset"));
}


int SequencerWindowGui::getCountInMode()
{
    return countIn;
}

void SequencerWindowGui::setCountIn(int i)
{
	if (i < 0 || i > 2) {
		return;
	}
    countIn = i;
    setChanged();
    notifyObservers(string("countin"));
}

void SequencerWindowGui::setInPlay(bool b)
{
	if (inPlay == b) {
		return;
	}
    inPlay = b;
    setChanged();
    notifyObservers(string("inplay"));
}

bool SequencerWindowGui::getInPlay()
{
    return inPlay;
}

int SequencerWindowGui::getRate()
{
    return rate;
}

void SequencerWindowGui::setRate(int i)
{
    if(i < 0 || i > 7)
        return;

    rate = i;
    setChanged();
    notifyObservers(string("rate"));
}

void SequencerWindowGui::setWaitForKey(bool b)
{
    if(waitForKey == b)
        return;

    waitForKey = b;
    setChanged();
    notifyObservers(string("waitforkey"));
}

bool SequencerWindowGui::isWaitForKeyEnabled()
{
    return waitForKey;
}

void SequencerWindowGui::setInRec(bool b)
{
    if(inRec == b)
        return;

    inRec = b;
    setChanged();
    notifyObservers(string("inrec"));
}

bool SequencerWindowGui::getInRec()
{
    return inRec;
}

int SequencerWindowGui::getClickVolume()
{
    return clickVolume;
}

void SequencerWindowGui::setClickVolume(int i)
{
    if(i < 0 || i > 100)
        return;

    clickVolume = i;
    setChanged();
    notifyObservers(string("clickvolume"));
}

int SequencerWindowGui::getClickOutput()
{
    return clickOutput;
}

void SequencerWindowGui::setClickOutput(int i)
{
    if(i < 0 || i > 8)
        return;

    clickOutput = i;
    setChanged();
    notifyObservers(string("clickoutput"));
}

int SequencerWindowGui::getMetronomeSound()
{
    return metronomeSound;
}

void SequencerWindowGui::setMetronomeSound(int i)
{
    if(i < 0 || i > 4)
        return;

    metronomeSound = i;
    setChanged();
    notifyObservers(string("metronomesound"));
}

int SequencerWindowGui::getAccentNote()
{
    return accentNote;
}

void SequencerWindowGui::setAccentNote(int i)
{
    if(i < 35 || i > 98)
        return;

    accentNote = i;
    setChanged();
    notifyObservers(string("accentnote"));
}

int SequencerWindowGui::getAccentVelo()
{
    return accentVelo;
}

void SequencerWindowGui::setAccentVelo(int i)
{
    if(i < 0 || i > 127)
        return;

    accentVelo = i;
    setChanged();
    notifyObservers(string("accentvelo"));
}

int SequencerWindowGui::getNormalNote()
{
    return normalNote;
}

void SequencerWindowGui::setNormalNote(int i)
{
    if(i < 35 || i > 98)
        return;

    normalNote = i;
    setChanged();
    notifyObservers(string("normalnote"));
}

int SequencerWindowGui::getNormalVelo()
{
    return normalVelo;
}

void SequencerWindowGui::setNormalVelo(int i)
{
    if(i < 0 || i > 127)
        return;

    normalVelo = i;
    setChanged();
    notifyObservers(string("normalvelo"));
}

bool SequencerWindowGui::isNotePassEnabled()
{
    return notePassEnabled;
}

void SequencerWindowGui::setNotePassEnabled(bool b)
{
    notePassEnabled = b;
}

bool SequencerWindowGui::isPitchBendPassEnabled()
{
    return pitchBendPassEnabled;
}

void SequencerWindowGui::setPitchBendPassEnabled(bool b)
{
    pitchBendPassEnabled = b;
}

bool SequencerWindowGui::isPgmChangePassEnabled()
{
    return pgmChangePassEnabled;
}

void SequencerWindowGui::setPgmChangePassEnabled(bool b)
{
    pgmChangePassEnabled = b;
}

bool SequencerWindowGui::isChPressurePassEnabled()
{
    return chPressurePassEnabled;
}

void SequencerWindowGui::setChPressurePassEnabled(bool b)
{
    chPressurePassEnabled = b;
}

bool SequencerWindowGui::isPolyPressurePassEnabled()
{
    return polyPressurePassEnabled;
}

void SequencerWindowGui::setPolyPressurePassEnabled(bool b)
{
    polyPressurePassEnabled = b;
}

bool SequencerWindowGui::isExclusivePassEnabled()
{
    return exclusivePassEnabled;
}

void SequencerWindowGui::setExclusivePassEnabled(bool b)
{
    exclusivePassEnabled = b;
}

int SequencerWindowGui::getTapAvg()
{
    return tapAvg;
}

void SequencerWindowGui::setTapAvg(int i)
{
    tapAvg = i;
}

bool SequencerWindowGui::isPgmChangeToSeqEnabled()
{
    return pgmChangeToSeqEnabled;
}

void SequencerWindowGui::setPgmChangeToSeqEnabled(bool b)
{
    pgmChangeToSeqEnabled = b;
}
