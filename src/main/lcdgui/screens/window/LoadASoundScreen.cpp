#include "LoadASoundScreen.hpp"

#include <sampler/Pad.hpp>

#include <sequencer/Track.hpp>

#include <lcdgui/screens/LoadScreen.hpp>

#include <mpc/MpcStereoMixerChannel.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace std;

LoadASoundScreen::LoadASoundScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "load-a-sound", layerIndex)
{
}

void LoadASoundScreen::open()
{
	init();
	auto loadScreen = mpc.screens->get<LoadScreen>("load");
	findLabel("filename").lock()->setText("File:" + loadScreen->getSelectedFileName());
    assignToNote = mpc.getNote();
	displayAssignToNote();
	mpc.addObserver(this); // Subscribe to "note" messages
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
		auto newAssignToNote = assignToNote + i;

        if (newAssignToNote < 34)
        {
            newAssignToNote = 34;
        }
        else if (newAssignToNote > 98)
        {
            newAssignToNote = 98;
        }

        if (newAssignToNote == 34)
        {
            mpc.setNote(35);
            assignToNote = newAssignToNote;
            displayAssignToNote();
        }
        else
        {
            mpc.setNote(newAssignToNote);
        }
    }
}

void LoadASoundScreen::function(int i)
{
	init();
	auto controls = mpc.getControls().lock();
	
	switch (i)
	{
	case 2:
	{
		if (controls->isF3Pressed())
			return;

		controls->setF3Pressed(true);
		auto s = sampler.lock()->getPreviewSound().lock();
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
			end *= 0.5;

        sampler.lock()->playPreviewSample(start, end, loopTo, overlapMode);
		break;
	}
	case 3:
		sampler.lock()->finishBasicVoice(); // Here we make sure the sound is not being played, so it can be removed from memory.
		sampler.lock()->deleteSound(sampler.lock()->getPreviewSound());
		openScreen("load");
		break;
	case 4:
		keepSound();
		openScreen("load");
		break;
	}
}

void LoadASoundScreen::keepSound()
{
    if (assignToNote != 34)
    {
        auto sequencer = mpc.getSequencer().lock();
        auto sequence = sequencer->getActiveSequence().lock();
        auto sampler = mpc.getSampler();
        auto track = sequence->getTrack(sequencer->getActiveTrackIndex()).lock();

        auto bus = track->getBus();
        auto programNumber = sampler.lock()->getDrumBusProgramNumber(bus);
        auto program = sampler.lock()->getProgram(programNumber);
        auto sound = sampler.lock()->getPreviewSound();
        auto noteParameters = sampler.lock()->getLastNp(program.lock().get());
        noteParameters->setSoundIndex(sampler.lock()->getSoundCount() - 1);
        auto mixerChannel = noteParameters->getStereoMixerChannel().lock();
        mixerChannel->setStereo(!sound.lock()->isMono());
    }
}

void LoadASoundScreen::displayAssignToNote()
{
	init();
	auto padIndex = program.lock()->getPadIndexFromNote(assignToNote);
	auto padName = sampler.lock()->getPadName(padIndex);
	auto noteName = string(assignToNote == 34 ? "--" : to_string(assignToNote));
	findField("assign-to-note").lock()->setText(noteName + "/" + padName);
}

void LoadASoundScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<string>(message);
	if (msg == "note")
	{
        assignToNote = mpc.getNote();
		displayAssignToNote();
	}
}
