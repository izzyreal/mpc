#include "LoadASoundScreen.hpp"

#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>

#include <sequencer/Track.hpp>

#include <lcdgui/screens/LoadScreen.hpp>

#include <mpc/MpcStereoMixerChannel.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace std;

LoadASoundScreen::LoadASoundScreen(const int layerIndex)
	: ScreenComponent("load-a-sound", layerIndex)
{
}

void LoadASoundScreen::open()
{
	if (mpc.getNote() < 34)
	{
		init();
		mpc.setPadAndNote(-1, 34);
	}
	else if (mpc.getNote() > 98)
	{
		init();
		auto pad = program.lock()->getPadIndexFromNote(98);
		mpc.setPadAndNote(pad, 98);
	}


	auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));
	findLabel("filename").lock()->setText("File:" + loadScreen->getSelectedFileName());
	displayAssignToNote();
	mpc.addObserver(this); // Subscribe to "padandnote" messages
}

void LoadASoundScreen::close()
{
	mpc.deleteObserver(this);
}

void LoadASoundScreen::turnWheel(int i)
{
	init();

	if (param.compare("assign-to-note") == 0)
	{
		auto nextNn = mpc.getNote() + i;
		auto nextPn = program.lock()->getPadIndexFromNote(nextNn);
		mpc.setPadAndNote(nextPn, nextNn);
	}
}

void LoadASoundScreen::function(int i)
{
	init();
	auto controls = Mpc::instance().getControls().lock();
	
	switch (i)
	{
	case 2:
	{
		if (controls->isF3Pressed()) {
			return;
		}

		controls->setF3Pressed(true);
		//samplerstopAllVoices();
		auto s = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getPreviewSound().lock());
		auto start = s->getStart();
		auto end = s->getSampleData()->size();
		auto loopTo = -1;
		auto overlapMode = 1;

		if (s->isLoopEnabled())
		{
			loopTo = s->getLoopTo();
			overlapMode = 2;
		}
		
		if (!s->isMono())
		{
			end *= 0.5;
		}
		sampler.lock()->playPreviewSample(start, end, loopTo, overlapMode);
		break;
	}
	case 3:
		sampler.lock()->finishBasicVoice(); // Here we make sure the sound is not being played, so it can be removed from memory.
		sampler.lock()->deleteSound(dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getPreviewSound().lock()));
		ls.lock()->openScreen("load");
		break;
	case 4:
		keepSound();
		ls.lock()->openScreen("load");
		break;
	}
}

void LoadASoundScreen::keepSound()
{
	auto sequencer = Mpc::instance().getSequencer().lock();
	auto sequence = sequencer->getActiveSequence().lock();
	auto sampler = Mpc::instance().getSampler();
	auto track = sequence->getTrack(sequencer->getActiveTrackIndex()).lock();

	auto bus = track->getBusNumber();
	auto programNumber = sampler.lock()->getDrumBusProgramNumber(bus);
	auto program = sampler.lock()->getProgram(programNumber);
	auto sound = sampler.lock()->getPreviewSound();

	if (mpc.getNote() != 34)
	{
		auto noteParameters = dynamic_cast<mpc::sampler::NoteParameters*>(program.lock()->getNoteParameters(mpc.getNote()));
		noteParameters->setSoundNumber(sampler.lock()->getSoundCount() - 1);

		if (sound.lock()->isLoopEnabled())
		{
			noteParameters->setVoiceOverlap(2);
		}

		auto pn = program.lock()->getPadIndexFromNote(mpc.getNote());

		if (pn != -1)
		{
			auto pad = dynamic_pointer_cast<mpc::sampler::Program>(program.lock())->getPad(pn);
			auto mixerChannel = pad->getStereoMixerChannel().lock();
			
			if (sound.lock()->isMono())
			{
				mixerChannel->setStereo(false);
			}
			else
			{
				mixerChannel->setStereo(true);
			}
		}
	}
}

void LoadASoundScreen::displayAssignToNote()
{
	init();
	auto note = mpc::Mpc::instance().getNote();
	auto padIndex = program.lock()->getPadIndexFromNote(note);
	auto padName = string(padIndex == -1 ? "OFF" : sampler.lock()->getPadName(padIndex));
	auto noteName = string(note == 34 ? "--" : to_string(note));
	findField("assign-to-note").lock()->setText(noteName + "/" + padName);
}

void LoadASoundScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<string>(message);
	if (msg.compare("padandnote") == 0)
	{
		displayAssignToNote();
	}
}
