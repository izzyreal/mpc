#include "sequencer/SeqUtil.hpp"

#include "Mpc.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/TimeSignature.hpp"

#include <cmath>

using namespace mpc::sequencer;

int SeqUtil::getTickFromBar(int i, Sequence* s, int position)
{
	if (i < 0)
		return 0;

	auto difference = i - getBarFromTick(s, position);
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));

	if (position + (difference * denTicks * 4) > s->getLastTick())
		position = s->getLastTick();
	else
		position = position + (difference * denTicks * 4);

    return position;
}

int SeqUtil::getBarFromTick(Sequence* s, int position)
{
	if (position == 0)
		return 0;

	auto ts = s->getTimeSignature();
	auto num = ts.getNumerator();
	auto den = ts.getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	auto bar = (int)(floor(position / (denTicks * num)));

    return bar;
}

double SeqUtil::secondsPerTick(const double tempo)
{
    return 60.0 / tempo / 96.0;
}

double SeqUtil::ticksPerSecond(const double tempo)
{

    auto bps = tempo / 60.0;
    return bps * 96.0;
}

double SeqUtil::ticksToFrames(double ticks, const double tempo, int sr)
{
	return (ticks * secondsPerTick(tempo) * sr);
}

double SeqUtil::sequenceFrameLength(Sequence* seq, int firstTick, int lastTick, int sr)
{
	double result = 0;
	auto lastTceTick = firstTick;
    auto tempoChangeEvents = seq->getTempoChangeEvents();
	auto tceSize = tempoChangeEvents.size();
    std::shared_ptr<TempoChangeEvent> lastTce;

	if (tceSize == 0)
	{
		result = ticksToFrames(lastTick - firstTick, seq->getInitialTempo(), sr);
		return result;
	}
	else
	{
		auto firstTceTick = tempoChangeEvents[0]->getTick();

        if (firstTick < firstTceTick)
			result = ticksToFrames(firstTceTick - firstTick, seq->getInitialTempo(), sr);
	}

	for (int i = 0; i < tceSize - 1; i++)
	{
		auto nextTce = tempoChangeEvents[i + 1];

		if (firstTick > nextTce->getTick())
			continue;

		if (lastTick < nextTce->getTick())
		{
			lastTce = nextTce;
			break;
		}

		auto tce = tempoChangeEvents[i];
		result += ticksToFrames(nextTce->getTick() - lastTceTick, tce->getTempo(), sr);
		lastTceTick = nextTce->getTick();
	}

	if (!lastTce)
	{
		lastTce = tempoChangeEvents[0];
	}

	result += ticksToFrames(lastTick - lastTce->getTick(), lastTce->getTempo(), sr);
	return (int)(ceil(result));
}

int SeqUtil::loopFrameLength(Sequence* seq, int sr)
{
    return static_cast<int>(sequenceFrameLength(seq, seq->getLoopStart(), seq->getLoopEnd(), sr));
}

int SeqUtil::songFrameLength(Song* song, Sequencer* sequencer, int sr)
{
	double result = 0;
	auto steps = song->getStepCount();

	for (int i = 0; i < steps; i++)
	{
		for (int j = 0; j < song->getStep(i).lock()->getRepeats(); j++)
		{
			auto seq = sequencer->getSequence(song->getStep(i).lock()->getSequence()).get();
			result += sequenceFrameLength(seq, 0, seq->getLastTick(), sr);
		}
	}

	return static_cast<int>(result);
}
void SeqUtil::setTimeSignature(Sequence* sequence, int firstBarIndex, int tsLastBarIndex, int num, int den)
{
	for (int i = firstBarIndex; i <= tsLastBarIndex; i++)
		setTimeSignature(sequence, i, num, den);
}

void SeqUtil::setTimeSignature(Sequence* sequence, int bar, int num, int den)
{
	auto newDenTicks = 96 * (4.0 / den);
    std::vector<int> newBarLengths(999);
	auto lastBar = sequence->getLastBarIndex();

	for (int i = 0; i < 999; i++)
		newBarLengths[i] = sequence->getBarLengthsInTicks()[i];

	newBarLengths[bar] = newDenTicks * num;

	sequence->getNumerators()[bar] = num;
	sequence->getDenominators()[bar] = den;

	if (bar == sequence->getLastBarIndex())
	{
		sequence->getNumerators()[sequence->getLastBarIndex() + 1] = num;
		sequence->getDenominators()[sequence->getLastBarIndex() + 1] = den;
	}
	else
	{
		sequence->getNumerators()[lastBar + 1] = sequence->getNumerators()[lastBar];
		sequence->getDenominators()[lastBar + 1] = sequence->getDenominators()[lastBar];
	}

	std::vector<int> oldBarStartPos(999);
	oldBarStartPos[0] = 0;

	for (int i = bar; i < 999; i++)
	{
		if (i == 0)
		{
			oldBarStartPos[i] = 0;
			continue;
		}

		oldBarStartPos[i] = oldBarStartPos[i - 1] + sequence->getBarLengthsInTicks()[i - 1];
	}

	std::vector<int> newBarStartPos(999);

	for (int i = bar; i < 999; i++)
	{
		if (i == 0)
		{
			newBarStartPos[i] = 0;
			continue;
		}

		newBarStartPos[i] = newBarStartPos[i - 1] + newBarLengths[i - 1];
	}

	for (auto& t : sequence->getTracks())
	{
		if (t->getIndex() == 64 || t->getIndex() == 65)
			continue;

        std::vector<std::shared_ptr<Event>> toRemove;
		bool keep;

		for (auto& event : t->getEvents())
		{
			keep = false;

			for (int i = 0; i < bar; i++)
			{
				if (event->getTick() >= oldBarStartPos[i] && event->getTick() < (oldBarStartPos[i] + sequence->getBarLengthsInTicks()[i]))
				{
					keep = true;
					break;
				}
			}

			for (int i = bar; i < 999; i++)
			{
				if (event->getTick() >= oldBarStartPos[i] && event->getTick() < (oldBarStartPos[i] + sequence->getBarLengthsInTicks()[i]))
				{
					event->setTick(event->getTick() - (oldBarStartPos[i] - newBarStartPos[i]));
					keep = true;
					break;
				}
			}

			if (!keep)
				toRemove.push_back(event);
		}

		for (auto& e : toRemove)
			t->removeEvent(e);
	}

    sequence->setBarLengths(newBarLengths);
}

int SeqUtil::setBar(int i, Sequence* sequence, int position)
{
	if (i < 0)
		return 0;

	auto difference = i - SeqUtil::getBar(sequence, position);
	auto den = sequence->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));

	if (position + (difference * denTicks * 4) > sequence->getLastTick())
		position = sequence->getLastTick();
	else
		position = position + (difference * denTicks * 4);

	return position;
}

int SeqUtil::setBeat(int i, Sequence* s, int position)
{
	if (i < 0)
		i = 0;

	auto difference = i - SeqUtil::getBeat(s, position);
	auto ts = s->getTimeSignature();
	auto num = ts.getNumerator();

	if (i >= num)
		return position;

	auto den = ts.getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));

	if (position + (difference * denTicks) > s->getLastTick())
		position = s->getLastTick();
	else
		position = position + (difference * denTicks);

	return position;
}

int SeqUtil::setClock(int i, Sequence* s, int position)
{
	if (i < 0)
		i = 0;

	auto difference = i - getClock(s, position);
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));

	if (i > denTicks - 1)
		return position;

	if (position + difference > s->getLastTick())
		position = s->getLastTick();
	else
		position = position + difference;

	return position;
}

int SeqUtil::getBar(Sequence* s, int position)
{
	if (position == 0)
		return 0;

	auto ts = s->getTimeSignature();
	auto num = ts.getNumerator();
	auto den = ts.getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	auto bar = (int)(floor(position / (denTicks * num)));
	return bar;
}

int SeqUtil::getBeat(Sequence* s, int position)
{
	if (position == 0)
		return 0;

	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));
	auto beat = (int)(floor(position / (denTicks)));
	beat = beat % den;
	return beat;
}

int SeqUtil::getClock(Sequence* s, int position)
{
	auto den = s->getTimeSignature().getDenominator();
	auto denTicks = (int)(96 * (4.0 / den));

	if (position == 0)
		return 0;

	auto clock = (int)(position % (denTicks));
	return clock;
}

void SeqUtil::copyBars(mpc::Mpc& mpc, uint8_t fromSeqIndex, uint8_t toSeqIndex, uint8_t copyFirstBar, uint8_t copyLastBar, uint8_t copyCount, uint8_t copyAfterBar)
{
    const auto sequencer = mpc.getSequencer();

    auto fromSequence = sequencer->getSequence(fromSeqIndex);

    if (!fromSequence->isUsed())
    {
        return;
    }

    auto toSequence = sequencer->getSequence(toSeqIndex);
    auto numberOfBars = (copyLastBar - copyFirstBar + 1) * copyCount;

    if (numberOfBars > 999)
    {
        numberOfBars = 999;
    }

    if (!toSequence->isUsed())
    {
        toSequence->init(numberOfBars - 1);
    }
    else
    {
        if (toSequence->getLastBarIndex() + numberOfBars > 998)
        {
            numberOfBars = 998 - toSequence->getLastBarIndex();
        }

        toSequence->insertBars(numberOfBars, copyAfterBar);
    }

    int copyCounter = 0;

    for (int i = 0; i < numberOfBars; i++)
    {
        toSequence->setTimeSignature(i + copyAfterBar, fromSequence->getNumerator(copyCounter + copyFirstBar), fromSequence->getDenominator(copyCounter + copyFirstBar));
        copyCounter++;

        if (copyCounter >= copyCount)
            copyCounter = 0;
    }

    auto firstTick = 0;
    auto lastTick = 0;
    auto firstTickOfToSeq = 0;

    for (int i = 0; i < 999; i++)
    {
        if (i == copyFirstBar)
            break;

        firstTick += fromSequence->getBarLengthsInTicks()[i];
    }

    for (int i = 0; i < 999; i++)
    {
        lastTick += fromSequence->getBarLengthsInTicks()[i];

        if (i == copyLastBar)
        {
            break;
        }
    }

    for (int i = 0; i < 999; i++)
    {
        if (i == copyAfterBar)
            break;

        firstTickOfToSeq += toSequence->getBarLengthsInTicks()[i];
    }

    auto segmentLengthTicks = lastTick - firstTick;
    auto offset = firstTickOfToSeq - firstTick;

    for (int i = 0; i < 64; i++)
    {
        auto t1 = fromSequence->getTrack(i);

        if (!t1->isUsed())
        {
            continue;
        }

        auto t1Events = t1->getEventRange(firstTick, lastTick);
        auto t2 = toSequence->getTrack(i);

        if (!t2->isUsed())
        {
            t2->setUsed(true);
        }

        auto toSeqLastTick = toSequence->getLastTick();

        for (auto& event : t1Events)
        {
            auto firstCopyTick = event->getTick() + offset;

            if (firstCopyTick >= toSeqLastTick)
            {
                // The events in Track are ordered by tick, so this and
                // any following event copies would be out of bounds.
                break;
            }

            for (auto k = 0; k < copyCount; k++)
            {
                auto tick = firstCopyTick + (k * segmentLengthTicks);

                // We do a more specific exit-check here, since within-bounds
                // copies may be followed by out-of-bounds ones.
                if (tick >= toSeqLastTick)
                {
                    break;
                }

                t2->cloneEventIntoTrack(event, tick);
            }
        }
    }
}