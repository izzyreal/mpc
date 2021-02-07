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
using namespace std;

bool WithTimesAndNotes::checkAllTimes(mpc::Mpc& mpc, int notch, Sequence* seq)
{
	auto sequence = seq != nullptr ? seq : mpc.getSequencer().lock()->getActiveSequence().lock().get();
	auto param = mpc.getLayeredScreen().lock()->getFocus();

	if (param.compare("time0") == 0)
	{
		setTime0(SeqUtil::getTickFromBar((SeqUtil::getBarFromTick(sequence, time0)) + notch, sequence, time0));
		return true;
	}
	else if (param.compare("time1") == 0)
	{
		setTime0(SeqUtil::setBeat((SeqUtil::getBeat(sequence, time0)) + notch, sequence, time0));
		return true;
	}
	else if (param.compare("time2") == 0)
	{
		setTime0(SeqUtil::setClock((SeqUtil::getClock(sequence, time0)) + notch, sequence, time0));
		return true;
	}
	else if (param.compare("time3") == 0)
	{
		setTime1(SeqUtil::getTickFromBar((SeqUtil::getBarFromTick(sequence, time1)) + notch, sequence, time1));
		return true;
	}
	else if (param.compare("time4") == 0)
	{
		setTime1(SeqUtil::setBeat((SeqUtil::getBeat(sequence, time1)) + notch, sequence, time1));
		return true;
	}
	else if (param.compare("time5") == 0)
	{
		setTime1(SeqUtil::setClock((SeqUtil::getClock(sequence, time1)) + notch, sequence, time1));
		return true;
	}

	return false;
}

bool WithTimesAndNotes::checkAllTimesAndNotes(mpc::Mpc& mpc, int notch, Sequence* seq, Track* _track)
{
	auto param = mpc.getLayeredScreen().lock()->getFocus();

	auto timesHaveChanged = checkAllTimes(mpc, notch, seq);
	auto notesHaveChanged = false;

	if (param.compare("note0") == 0)
	{
		auto track = mpc.getSequencer().lock()->getActiveTrack().lock().get();

		if (_track != nullptr)
			track = _track;

		if (track->getBus() != 0)
		{
			auto note = mpc.getNote() + notch;
			auto mpcSoundPlayerChannel = mpc.getSampler().lock()->getDrum(track->getBus() - 1);
			auto program = mpc.getSampler().lock()->getProgram(mpcSoundPlayerChannel->getProgram()).lock();
			auto pad = program->getPadIndexFromNote(note);
			mpc.setPadAndNote(pad, note);
			displayDrumNotes();
		}
		else
		{
			setNote0(note0 + notch);
		}

		notesHaveChanged = true;
	}
	else if (param.compare("note1") == 0)
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

void WithTimesAndNotes::setTime0(int time0)
{
	this->time0 = time0;

	if (time0 > time1)
		time1 = time0;

	displayTime();
}

void WithTimesAndNotes::setTime1(int time1)
{
	this->time1 = time1;

	if (time1 < time0)
		time0 = time1;

	displayTime();
}
