#include "WithTimesAndNotes.hpp"

#include <sequencer/Sequencer.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>

#include <sampler/Sampler.hpp>
#include <sampler/Program.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

bool WithTimesAndNotes::checkAllTimes(mpc::Mpc& mpc, int notch, Sequence* seq)
{
	auto sequence = seq != nullptr ? seq : mpc.getSequencer()->getActiveSequence().get();
	auto param = mpc.getLayeredScreen()->getFocus();

	if (param == "time0")
	{
		setTime0(SeqUtil::getTickFromBar((SeqUtil::getBarFromTick(sequence, time0)) + notch, sequence, time0));
		return true;
	}
	else if (param == "time1")
	{
		setTime0(SeqUtil::setBeat((SeqUtil::getBeat(sequence, time0)) + notch, sequence, time0));
		return true;
	}
	else if (param == "time2")
	{
		setTime0(SeqUtil::setClock((SeqUtil::getClock(sequence, time0)) + notch, sequence, time0));
		return true;
	}
	else if (param == "time3")
	{
		setTime1(SeqUtil::getTickFromBar((SeqUtil::getBarFromTick(sequence, time1)) + notch, sequence, time1));
		return true;
	}
	else if (param == "time4")
	{
		setTime1(SeqUtil::setBeat((SeqUtil::getBeat(sequence, time1)) + notch, sequence, time1));
		return true;
	}
	else if (param == "time5")
	{
		setTime1(SeqUtil::setClock((SeqUtil::getClock(sequence, time1)) + notch, sequence, time1));
		return true;
	}

	return false;
}

bool WithTimesAndNotes::checkAllTimesAndNotes(mpc::Mpc& mpc, int notch, Sequence* seq, Track* _track)
{
	auto param = mpc.getLayeredScreen()->getFocus();

	auto timesHaveChanged = checkAllTimes(mpc, notch, seq);
	auto notesHaveChanged = false;

	if (param == "note0")
	{
		auto track = mpc.getSequencer()->getActiveTrack().get();

		if (_track != nullptr)
			track = _track;

		if (track->getBus() != 0)
		{
			auto note = note0 + notch;

            if (note < 34 || note > 98)
            {
                return true;
            }

			note0 = note;

            displayDrumNotes();
		}
		else
		{
			setNote0(note0 + notch);
		}

		notesHaveChanged = true;
	}
	else if (param == "note1")
	{
		setNote1(note1 + notch);
		notesHaveChanged = true;
	}

	return timesHaveChanged || notesHaveChanged;
}

void WithTimesAndNotes::setNote0(int i)
{
	if (i < 0 || i > 127)
		return;

	note0 = i;

	if (note0 > note1)
		note1 = note0;

	displayNotes();
}

void WithTimesAndNotes::setNote1(int i)
{
	if (i < 0 || i > 127)
		return;

	note1 = i;

	if (note1 < note0)
		note0 = note1;

	displayNotes();
}

void WithTimesAndNotes::setTime0(int newTime0)
{
	time0 = newTime0;

	if (newTime0 > time1)
		time1 = newTime0;

	displayTime();
}

void WithTimesAndNotes::setTime1(int newTime1)
{
	time1 = newTime1;

	if (newTime1 < time0)
		time0 = newTime1;

	displayTime();
}
