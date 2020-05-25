#include <controls/disk/window/LoadASoundControls.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <ui/sampler/SamplerGui.hpp>
#include <ui/Uis.hpp>
#include <ui/sampler/SoundGui.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>

#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

#include <mpc/MpcStereoMixerChannel.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

void LoadASoundControls::turnWheel(int i)
{
	init();
	if (param.compare("assigntonote") == 0)
	{
		auto nextNn = samplerGui->getNote() + i;
		auto nextPn = program.lock()->getPadNumberFromNote(nextNn);
		samplerGui->setPadAndNote(nextPn, nextNn);
	}
}

void LoadASoundControls::function(int i)
{
	init();
	auto controls = Mpc::instance().getControls().lock();
	shared_ptr<mpc::sampler::Sound> s;
	int start;
	int end;
	int loopTo;
	int overlapMode;
	
	switch (i) {
	case 2:
		
		if (controls->isF3Pressed()) {
			return;
		}

		controls->setF3Pressed(true);
		//samplerstopAllVoices();
		s = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getPreviewSound().lock());
		start = s->getStart();
		end = s->getSampleData()->size();
		loopTo = -1;
		overlapMode = 1;
		if (s->isLoopEnabled()) {
			loopTo = s->getLoopTo();
			overlapMode = 2;
		}
		if (!s->isMono()) {
			end /= 2;
		}
		sampler.lock()->playPreviewSample(start, end, loopTo, overlapMode);
		break;
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

void LoadASoundControls::keepSound()
{
	auto sequencer = Mpc::instance().getSequencer().lock();
	auto sequence = sequencer->getActiveSequence().lock();
	auto sampler = Mpc::instance().getSampler();
	auto track = sequence->getTrack(sequencer->getActiveTrackIndex()).lock();

	auto bus = track->getBusNumber();
	auto programNumber = sampler.lock()->getDrumBusProgramNumber(bus);
	auto program = sampler.lock()->getProgram(programNumber);
	auto sound = sampler.lock()->getPreviewSound();

	if (samplerGui->getNote() != 34)
	{
		auto noteParameters = dynamic_cast<mpc::sampler::NoteParameters*>(program.lock()->getNoteParameters(samplerGui->getNote()));
		noteParameters->setSoundNumber(sampler.lock()->getSoundCount() - 1);

		if (sound.lock()->isLoopEnabled())
		{
			noteParameters->setVoiceOverlap(2);
		}

		auto pn = program.lock()->getPadNumberFromNote(samplerGui->getNote());

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
