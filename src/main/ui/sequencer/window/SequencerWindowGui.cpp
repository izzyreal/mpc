#include "SequencerWindowGui.hpp"

#include <sequencer/Event.hpp>
#include <sequencer/TempoChangeEvent.hpp>

using namespace mpc::ui::sequencer::window;
using namespace std;

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
