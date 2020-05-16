#include "SequencerWindowGui.hpp"

#include <ui/sequencer/window/MultiRecordingSetupLine.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/TempoChangeEvent.hpp>

using namespace mpc::ui::sequencer::window;
using namespace std;

SequencerWindowGui::SequencerWindowGui() 
{

	inPlay = true;
	inRec = true;
	receiveCh = 0;
	value = 1;
	clickVolume = 100;
	accentVelo = 127;
	accentNote = 35;
	normalVelo = 64;
	normalNote = 35;

	mrsLines = vector<MultiRecordingSetupLine*>(34);
	for (int i = 0; i < 34; i++) {
		auto mrsLine = new MultiRecordingSetupLine(i);
		mrsLine->setTrack(i);
		mrsLine->setOut(0);
		mrsLines[i] = mrsLine;
	}
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

int SequencerWindowGui::getMidiNote0()
{
    return midiNote0;
}

void SequencerWindowGui::setMidiNote0(int i)
{
    if (i < 0 || i > 127)
    {
        return;
    }

    midiNote0 = i;
    setChanged();
    notifyObservers(string("notes"));
}

int SequencerWindowGui::getMidiNote1()
{
    return midiNote1;
}

void SequencerWindowGui::setMidiNote1(int i)
{
    if (i < 0 || i > 127)
    {
        return;
    }

    midiNote1 = i;
    setChanged();
    notifyObservers(string("notes"));
}

int SequencerWindowGui::getTime0()
{
    return time0;
}

void SequencerWindowGui::setTime0(int time0)
{
    this->time0 = time0;
    if(time0 > time1)
        time1 = time0;

    setChanged();
    notifyObservers(string("time"));
}

int SequencerWindowGui::getTime1()
{
    return time1;
}

void SequencerWindowGui::setTime1(int time1)
{
    this->time1 = time1;
    if(time1 < time0)
        time0 = time1;

    setChanged();
    notifyObservers(string("time"));
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

bool SequencerWindowGui::getTransmitProgramChangesInThisTrack()
{
    return transmitProgramChangesInThisTrack;
}

void SequencerWindowGui::setTransmitProgramChangesInThisTrack(bool b)
{
    transmitProgramChangesInThisTrack = b;
    setChanged();
    notifyObservers(string("transmitprogramchangesinthistrack"));
}

vector<MultiRecordingSetupLine*>  SequencerWindowGui::getMrsLines()
{
    return mrsLines;
}

void SequencerWindowGui::setMrsYOffset(int i)
{
    if(i < 0) return;

    if(i + 3 > mrsLines.size())
        return;

    visibleMrsLines = vector<MultiRecordingSetupLine*>(3);
    mrsYOffset = i;
    for (auto j = 0; j < 3; j++) {
		visibleMrsLines[j] = mrsLines[mrsYOffset + j];
    }
    setChanged();
    notifyObservers(string("multirecordingsetup"));
}

int SequencerWindowGui::getMrsYOffset()
{
    return mrsYOffset;
}

void SequencerWindowGui::setMrsTrack(int inputNumber, int newTrackNumber)
{
    mrsLines[inputNumber]->setTrack(newTrackNumber);
    visibleMrsLines = vector<MultiRecordingSetupLine*>(3);
    for (auto j = 0; j < 3; j++) {
		visibleMrsLines[j] = mrsLines[mrsYOffset + j];
    }
    setChanged();
    notifyObservers(string("mrsline"));
}

void SequencerWindowGui::setMrsOut(int inputNumber, int newOutputNumber)
{
    mrsLines[inputNumber]->setOut(newOutputNumber);
    visibleMrsLines = vector<MultiRecordingSetupLine*>(3);
    for (auto j = 0; j < 3; j++) {
		visibleMrsLines[j] = mrsLines[mrsYOffset + j];
    }
    setChanged();
    notifyObservers(string("mrsline"));
}

void SequencerWindowGui::setVisbleMrsLines(vector<MultiRecordingSetupLine*>*  newVisibleMrsLines)
{
    visibleMrsLines = *newVisibleMrsLines;
}

vector<MultiRecordingSetupLine*>*  SequencerWindowGui::getVisibleMrsLines()
{
    return &visibleMrsLines;
}

void SequencerWindowGui::setReceiveCh(int i)
{
    if(i < -1 || i > 15)
        return;

    receiveCh = i;
    setChanged();
    notifyObservers(string("receivech"));
}

int SequencerWindowGui::getReceiveCh()
{
    return receiveCh;
}

void SequencerWindowGui::setProgChangeSeq(bool b)
{
    if(progChangeSeq == b)
        return;

    progChangeSeq = b;
    setChanged();
    notifyObservers(string("progchangeseq"));
}

bool SequencerWindowGui::getProgChangeSeq()
{
    return progChangeSeq;
}

void SequencerWindowGui::setSustainPedalToDuration(bool b)
{
    if(sustainPedalToDuration == b)
        return;

    sustainPedalToDuration = b;
    setChanged();
    notifyObservers(string("sustainpedaltoduration"));
}

bool SequencerWindowGui::isSustainPedalToDurationEnabled()
{
    return sustainPedalToDuration;
}

void SequencerWindowGui::setMidiFilterEnabled(bool b)
{
    if(midiFilter == b)
        return;

    midiFilter = b;
    setChanged();
    notifyObservers(string("midifilter"));
}

bool SequencerWindowGui::isMidiFilterEnabled()
{
    return midiFilter;
}

void SequencerWindowGui::setFilterType(int i)
{
    if(i < 0 || i > 134)
        return;

    filterType = i;
    setChanged();
    notifyObservers(string("type"));
}

int SequencerWindowGui::getMidiFilterType()
{
    return filterType;
}

void SequencerWindowGui::setPass(bool b)
{
    if(pass == b)
        return;

    pass = b;
    setChanged();
    notifyObservers(string("pass"));
}

bool SequencerWindowGui::getPass()
{
    return pass;
}

void SequencerWindowGui::setSoftThru(int i)
{
    if(i < 0 || i > 4)
        return;

    softThru = i;
    setChanged();
    notifyObservers(string("softthru"));
}

int SequencerWindowGui::getSoftThru()
{
    return softThru;
}

void SequencerWindowGui::setDeviceNumber(int i)
{
    if(i < 0 || i > 31)
        return;

    deviceNumber = i;
    setChanged();
    notifyObservers(string("devicenumber"));
}

int SequencerWindowGui::getDeviceNumber()
{
    return deviceNumber;
}

int SequencerWindowGui::getEditType()
{
    return editType;
}

void SequencerWindowGui::setEditType(int i)
{
    if(i < 0 || i > 3)
        return;

    editType = i;
    setChanged();
    notifyObservers(string("edittype"));
}

int SequencerWindowGui::getValue()
{
    return value;
}

void SequencerWindowGui::setValue(int i)
{
    if(i < 1 || i > 200)
        return;

    if(editType != 2 && i > 127)
        return;

    value = i;
    setChanged();
    notifyObservers(string("value"));
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

SequencerWindowGui::~SequencerWindowGui() {
	for (auto& m : mrsLines) {
		if (m != nullptr) {
			delete m;
		}
	}
}
