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

void WithTimesAndNotes::checkAllTimesAndNotes(int notch, Sequence* seq)
{
	auto& mpc = mpc::Mpc::instance();

	auto sequence = seq != nullptr ? seq : mpc.getSequencer().lock()->getActiveSequence().lock().get();
	auto param = mpc.getLayeredScreen().lock()->getFocus();

	if (param.compare("time0") == 0)
	{
		setTime0(SeqUtil::getTickFromBar((SeqUtil::getBarFromTick(sequence, time0)) + notch, sequence, time0));
	}
	else if (param.compare("time1") == 0)
	{
		setTime0(SeqUtil::setBeat((SeqUtil::getBeat(sequence, time0)) + notch, sequence, time0));
	}
	else if (param.compare("time2") == 0)
	{
		setTime0(SeqUtil::setClock((SeqUtil::getClock(sequence, time0)) + notch, sequence, time0));
	}
	else if (param.compare("time3") == 0)
	{
		setTime1(SeqUtil::getTickFromBar((SeqUtil::getBarFromTick(sequence, time1)) + notch, sequence, time1));
	}
	else if (param.compare("time4") == 0)
	{
		setTime1(SeqUtil::setBeat((SeqUtil::getBeat(sequence, time1)) + notch, sequence, time1));
	}
	else if (param.compare("time5") == 0)
	{
		setTime1(SeqUtil::setClock((SeqUtil::getClock(sequence, time1)) + notch, sequence, time1));
	}
	else if (param.compare("notes0") == 0)
	{

		auto track = mpc.getSequencer().lock()->getActiveTrack().lock();

		if (track->getBusNumber() != 0)
		{
			auto note = mpc.getNote() + notch;
			auto mpcSoundPlayerChannel = mpc.getSampler().lock()->getDrum(track->getBusNumber() - 1);
			auto program = dynamic_pointer_cast<mpc::sampler::Program>(mpc.getSampler().lock()->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
			auto pad = program->getPadIndexFromNote(note);
			mpc.setPadAndNote(pad, mpc.getNote());
		}
		else
		{
			setMidiNote0(getMidiNote0() + notch);
		}
	}
	else if (param.compare("notes1") == 0)
	{
		setMidiNote1(getMidiNote1() + notch);
	}
}

int WithTimesAndNotes::getMidiNote0()
{
	return note0;
}

void WithTimesAndNotes::setMidiNote0(int i)
{
	if (i < 0 || i > 127)
	{
		return;
	}

	note0 = i;
	displayNotes();
}

int WithTimesAndNotes::getMidiNote1()
{
	return note1;
}

void WithTimesAndNotes::setMidiNote1(int i)
{
	if (i < 0 || i > 127)
	{
		return;
	}

	note1 = i;
	displayNotes();
}

void WithTimesAndNotes::setTime0(int time0)
{
	this->time0 = time0;

	if (time0 > time1)
	{
		time1 = time0;
	}

	displayTime();
}

void WithTimesAndNotes::setTime1(int time1)
{
	this->time1 = time1;

	if (time1 < time0)
	{
		time0 = time1;
	}

	displayTime();
}
