#include <sequencer/SeqUtil.hpp>

#include <Mpc.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Step.hpp>
#include <sequencer/Song.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/TimeSignature.hpp>

#include <cmath>

using namespace mpc::sequencer;
using namespace std;

int SeqUtil::getTickFromBar(int i, Sequence* s, int position)
{

	if (i < 0) {
		return 0;
	}

	auto difference = i - getBarFromTick(s, position);
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

int SeqUtil::getBarFromTick(Sequence* s, int position)
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

double SeqUtil::secondsPerTick(BCMath tempo)
{   
    return 60.0 / tempo.toDouble() / 96.0;
}

double SeqUtil::ticksPerSecond(BCMath tempo)
{
    
    auto bps = tempo.toDouble() / 60.0;
    return bps * 96.0;
}

double SeqUtil::ticksToFrames(double ticks, BCMath tempo, float sr)
{   
	return (ticks * secondsPerTick(tempo) * sr);
}

double SeqUtil::framesToTicks(double d, BCMath tempo, float sr)
{
	return (d / sr) * (ticksPerSecond(tempo));
}

double SeqUtil::sequenceFrameLength(Sequence* seq, int firstTick, int lastTick, float sr)
{
	double result = 0;
	auto lastTceTick = firstTick;
	auto tceSize = seq->getTempoChangeEvents().size();
	weak_ptr<TempoChangeEvent> lastTce;

	if (tceSize == 0) {
		result = ticksToFrames(lastTick - firstTick, seq->getInitialTempo(), sr);
		return result;
	}
	else {
		auto firstTceTick = seq->getTempoChangeEvents()[0].lock()->getTick();
		if (firstTick < firstTceTick) {
			result = ticksToFrames(firstTceTick - firstTick, seq->getInitialTempo(), sr);
		}
	}

	for (int i = 0; i < tceSize - 1; i++) {
		auto nextTce = seq->getTempoChangeEvents()[i + 1].lock();
		if (firstTick > nextTce->getTick()) continue;

		if (lastTick < nextTce->getTick()) {
			lastTce = nextTce;
			break;
		}
		auto tce = seq->getTempoChangeEvents()[i].lock();
		result += ticksToFrames(nextTce->getTick() - lastTceTick, tce->getTempo(), sr);
		lastTceTick = nextTce->getTick();
	}
	auto lLastTce = lastTce.lock();
	if (!lLastTce) {
		lastTce = seq->getTempoChangeEvents()[0];
		lLastTce = lastTce.lock();
	}

	result += ticksToFrames(lastTick - lLastTce->getTick(), lLastTce->getTempo(), sr);
	return (int)(ceil(result));
	return 0;
}

int SeqUtil::loopFrameLength(Sequence* seq, float sr)
{
    return static_cast<int>(sequenceFrameLength(seq, seq->getLoopStart(), seq->getLoopEnd(), sr));
}

int SeqUtil::songFrameLength(Song* song, mpc::sequencer::Sequencer* sequencer, float sr)
{
	double result = 0;
	auto steps = song->getStepAmount();
	for (int i = 0; i < steps; i++) {
		for (int j = 0; j < song->getStep(i)->getRepeats(); j++) {
			auto seq = sequencer->getSequence(song->getStep(i)->getSequence()).lock().get();
			result += sequenceFrameLength(seq, 0, seq->getLastTick(), sr);
		}
	}
	return static_cast<int>(result);
}
void SeqUtil::setTimeSignature(Sequence* sequence, int firstBar, int tsLastBar, int num, int den)
{
	for (int i = firstBar; i < tsLastBar + 1; i++)
		setTimeSignature(sequence, i, num, den);
}

void SeqUtil::setTimeSignature(Sequence* sequence, int bar, int num, int den)
{
	auto newDenTicks = 96 * (4.0 / den);
	auto newBarLengths = vector<int>(999);
	auto lastBar = sequence->getLastBar();
	for (int i = 0; i < 999; i++) {
		newBarLengths[i] = (*sequence->getBarLengths())[i];
	}
	newBarLengths[bar] = newDenTicks * num;
	(*sequence->getNumerators())[bar] = num;
	(*sequence->getDenominators())[bar] = den;
	if (bar == sequence->getLastBar()) {
		(*sequence->getNumerators())[sequence->getLastBar() + 1] = num;
		(*sequence->getDenominators())[sequence->getLastBar() + 1] = den;
	}
	else {
		(*sequence->getNumerators())[lastBar + 1] = (*sequence->getNumerators())[lastBar];
		(*sequence->getDenominators())[lastBar + 1] = (*sequence->getDenominators())[lastBar];
	}
	auto oldBarStartPos = vector<int>(999);
	oldBarStartPos[0] = 0;
	for (int i = bar; i < 999; i++) {
		if (i == 0) {
			oldBarStartPos[i] = 0;
			continue;
		}
		oldBarStartPos[i] = oldBarStartPos[i - 1] + (*sequence->getBarLengths())[i - 1];
	}
	auto newBarStartPos = vector<int>(999);
	for (int i = bar; i < 999; i++) {
		if (i == 0) {
			newBarStartPos[i] = 0;
			continue;
		}
		newBarStartPos[i] = newBarStartPos[i - 1] + newBarLengths[i - 1];
	}

	for (auto& t : sequence->getTracks()) {
		auto lTrk = t.lock();
		if (lTrk->getTrackIndex() > 63)
			continue;

		vector<weak_ptr<Event>> toRemove;
		bool keep;
		for (auto& event : lTrk->getEvents()) {
			keep = false;
			auto e = event.lock();
			for (int i = 0; i < bar; i++) {
				if (e->getTick() >= oldBarStartPos[i] && e->getTick() < (oldBarStartPos[i] + (*sequence->getBarLengths())[i])) {
					keep = true;
					break;
				}
			}

			for (int i = bar; i < 999; i++) {
				if (e->getTick() >= oldBarStartPos[i] && e->getTick() < (oldBarStartPos[i] + (*sequence->getBarLengths())[i])) {
					e->setTick(e->getTick() - (oldBarStartPos[i] - newBarStartPos[i]));
					keep = true;
					break;
				}
			}
			if (!keep) toRemove.push_back(e);
		}

		for (auto& e : toRemove)
			lTrk->removeEvent(e);
	}

	for (int i = 0; i < 999; i++) {
		(*sequence->getBarLengths())[i] = newBarLengths[i];
	}
	sequence->initMetaTracks();
}

int SeqUtil::setBar(int i, mpc::sequencer::Sequence* sequence, int position)
{
	if (i < 0)
	{
		return 0;
	}

	auto difference = i - SeqUtil::getBar(sequence, position);
	auto den = sequence->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));

	if (position + (difference * denTicks * 4) > sequence->getLastTick())
	{
		position = sequence->getLastTick();
	}
	else {
		position = position + (difference * denTicks * 4);
	}
	return position;
}

int SeqUtil::setBeat(int i, mpc::sequencer::Sequence* s, int position)
{
	if (i < 0)
	{
		i = 0;
	}
	auto difference = i - SeqUtil::getBeat(s, position);
	auto ts = s->getTimeSignature();
	auto num = ts.getNumerator();

	if (i >= num)
	{
		return position;
	}

	auto den = ts.getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));

	if (position + (difference * denTicks) > s->getLastTick())
	{
		position = s->getLastTick();
	}
	else
	{
		position = position + (difference * denTicks);
	}
	return position;
}

int SeqUtil::setClock(int i, mpc::sequencer::Sequence* s, int position)
{
	if (i < 0)
	{
		i = 0;
	}

	auto difference = i - getClock(s, position);
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));

	if (i > denTicks - 1) {
		return position;
	}

	if (position + difference > s->getLastTick())
	{
		position = s->getLastTick();
	}
	else
	{
		position = position + difference;
	}
	return position;
}

int SeqUtil::getBar(mpc::sequencer::Sequence* s, int position)
{
	if (position == 0)
	{
		return 0;
	}
	auto ts = s->getTimeSignature();
	auto num = ts.getNumerator();
	auto den = ts.getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	auto bar = (int)(floor(position / (denTicks * num)));
	return bar;
}

int SeqUtil::getBeat(mpc::sequencer::Sequence* s, int position)
{
	if (position == 0)
	{
		return 0;
	}
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	auto beat = (int)(floor(position / (denTicks)));
	beat = beat % den;
	return beat;
}

int SeqUtil::getClock(mpc::sequencer::Sequence* s, int position)
{
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	if (position == 0)
	{
		return 0;
	}
	auto clock = (int)(position % (denTicks));
	return clock;
}
