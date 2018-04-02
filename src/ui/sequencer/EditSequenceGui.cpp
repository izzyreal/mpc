#include <ui/sequencer/EditSequenceGui.hpp>

#include <sequencer/Sequence.hpp>
#include <sequencer/TimeSignature.hpp>

#include <cmath>

using namespace mpc::ui::sequencer;
using namespace std;

EditSequenceGui::EditSequenceGui() 
{
	copies = 1;
	drumNote = 34;
	midiNote1 = 127;
	durationValue = 1;
	velocityValue = 1;
}

void EditSequenceGui::setEditFunctionNumber(int i)
{
    if (i < 0 || i > 3) return;
    editFunctionNumber = i;
    setChanged();
    notifyObservers(string("editfunction"));
}

int EditSequenceGui::getEditFunctionNumber()
{
    return editFunctionNumber;
}

int EditSequenceGui::getDrumNote()
{
    return drumNote;
}

void EditSequenceGui::setDrumNote(int i)
{
    if (i < 34 || i > 98) return;
    drumNote = i;
    setChanged();
    notifyObservers(string("drumnote"));
}

int EditSequenceGui::getMidiNote0()
{
    return midiNote0;
}

void EditSequenceGui::setMidiNote0(int i)
{
    if (i < 0 || i > 127) return;
    midiNote0 = i;
    setChanged();
    notifyObservers(string("midinote0"));
}

int EditSequenceGui::getMidiNote1()
{
    return midiNote1;
}

void EditSequenceGui::setMidiNote1(int i)
{
    if (i < 0 || i > 127) return;
    midiNote1 = i;
    setChanged();
    notifyObservers(string("midinote1"));
}

int EditSequenceGui::getFromSq()
{
    return fromSq;
}

void EditSequenceGui::setFromSq(int i)
{
    if (i < 0 || i > 98) return;
    fromSq = i;
    setChanged();
    notifyObservers(string("fromsq"));
}

int EditSequenceGui::getTr0()
{
    return tr0;
}

void EditSequenceGui::setTr0(int i)
{
    if (i < 0 || i > 63) return;
    tr0 = i;
    setChanged();
    notifyObservers(string("tr0"));
}

int EditSequenceGui::getToSq()
{
    return toSq;
}

void EditSequenceGui::setToSq(int i)
{
    if (i < 0 || i > 98) return;
    toSq = i;
    setChanged();
    notifyObservers(string("tosq"));
}

int EditSequenceGui::getTr1()
{
    return tr1;
}

void EditSequenceGui::setTr1(int i)
{
    if(i < 0 || i > 63) return;
    tr1 = i;
    setChanged();
    notifyObservers(string("tr1"));
}

bool EditSequenceGui::isModeMerge()
{
    return modeMerge;
}

void EditSequenceGui::setModeMerge(bool b)
{
    modeMerge = b;
    setChanged();
    notifyObservers(string("modevalue"));
}

int EditSequenceGui::getCopies()
{
    return copies;
}

void EditSequenceGui::setCopies(int i)
{
    if(i < 1 || i > 999) return;
    copies = i;
    setChanged();
    notifyObservers(string("copies"));
}

void EditSequenceGui::setDurationMode(int i)
{
    if(i < 0 || i > 3) return;

    durationMode = i;
    if(durationMode == 2 && durationValue > 200)
        setDurationValue(200);

    setChanged();
    notifyObservers(string("modevalue"));
}

int EditSequenceGui::getDurationMode()
{
    return durationMode;
}

void EditSequenceGui::setVelocityMode(int i)
{
    if(i < 0 || i > 3) return;
    velocityMode = i;
	if (velocityMode != 2 && velocityValue > 127) {
		setVelocityValue(127);
	}
    setChanged();
    notifyObservers(string("modevalue"));
}

int EditSequenceGui::getVelocityMode()
{
    return velocityMode;
}

void EditSequenceGui::setTransposeAmount(int i)
{
    if(i < -12 || i > 12) return;
    transposeAmount = i;
    setChanged();
    notifyObservers(string("modevalue"));
}

int EditSequenceGui::getTransposeAmount()
{
    return transposeAmount;
}

int EditSequenceGui::getDurationValue()
{
    return durationValue;
}

void EditSequenceGui::setDurationValue(int i)
{
    if(i < 1 || i > 9999) return;
    if(durationMode == 2 && i > 200) return;
    durationValue = i;
    setChanged();
    notifyObservers(string("copies"));
}

int EditSequenceGui::getVelocityValue()
{
    return velocityValue;
}

void EditSequenceGui::setVelocityValue(int i)
{
    if(i < 1 || i > 200) return;
    if(velocityMode != 2 && i > 127) return;
    velocityValue = i;
    setChanged();
    notifyObservers(string("copies"));
}

int EditSequenceGui::getTime0()
{
    return time0;
}

void EditSequenceGui::setTime0(int time0)
{
    this->time0 = time0;
    if(time0 > time1) time1 = time0;
    setChanged();
    notifyObservers(string("time"));
}

int EditSequenceGui::getTime1()
{
    return time1;
}

void EditSequenceGui::setTime1(int time1)
{
    this->time1 = time1;
    if(time1 < time0) time0 = time1;
    setChanged();
    notifyObservers(string("time"));
}

int EditSequenceGui::getStartTicks()
{
    return startTicks;
}

void EditSequenceGui::setStartTicks(int startTicks)
{
    this->startTicks = startTicks;
    setChanged();
    notifyObservers(string("start"));
}

int EditSequenceGui::setBarNumber(int i, mpc::sequencer::Sequence* s, int position)
{
	if (i < 0) return 0;
	auto difference = i - getBarNumber(s, position);
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	if (position + (difference * denTicks * 4) > s->getLastTick()) {
		position = s->getLastTick();
	}
	else {
		position = position + (difference * denTicks * 4);
	}
	return position;
}

int EditSequenceGui::setBeatNumber(int i, mpc::sequencer::Sequence* s, int position)
{
	if (i < 0) {
		i = 0;
	}
	auto difference = i - getBeatNumber(s, position);
	auto ts = s->getTimeSignature();
	auto num = ts.getNumerator();
	if (i >= num) {
		return position;
	}
	auto den = ts.getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	if (position + (difference * denTicks) > s->getLastTick()) {
		position = s->getLastTick();
	}
	else {
		position = position + (difference * denTicks);
	}
	return position;
}

int EditSequenceGui::setClockNumber(int i, mpc::sequencer::Sequence* s, int position)
{
	if (i < 0) i = 0;
	auto difference = i - getClockNumber(s, position);
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	if (i > denTicks - 1) return position;
	if (position + difference > s->getLastTick()) {
		position = s->getLastTick();
	}
	else {
		position = position + difference;
	}
	return position;
}

int EditSequenceGui::getBarNumber(mpc::sequencer::Sequence* s, int position)
{
	if (position == 0) {
		return 0;
	}
	auto ts = s->getTimeSignature();
	auto num = ts.getNumerator();
	auto den = ts.getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	auto bar = (int)(floor(position / (denTicks * num)));
	return bar;
}

int EditSequenceGui::getBeatNumber(mpc::sequencer::Sequence* s, int position)
{
	if (position == 0) {
		return 0;
	}
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	auto beat = (int)(floor(position / (denTicks)));
	beat = beat % den;
	return beat;
}

int EditSequenceGui::getClockNumber(mpc::sequencer::Sequence* s, int position)
{
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	if (position == 0) return 0;
	auto clock = (int)(position % (denTicks));
	return clock;
}
