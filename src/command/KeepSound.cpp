#include <command/KeepSound.hpp>

#include <ui/Uis.hpp>
#include <Mpc.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

#include <mpc/MpcStereoMixerChannel.hpp>

using namespace mpc::command;
using namespace std;

KeepSound::KeepSound(mpc::Mpc* mpc, weak_ptr<mpc::sampler::Sound> s) 
{
	this->mpc = mpc;
	auto uis = mpc->getUis().lock();
	samplerGui = uis->getSamplerGui();
	soundGui = uis->getSoundGui();
	sound = s;
}

void KeepSound::execute()
{
	auto sequencer = mpc->getSequencer().lock();
	auto sequence = sequencer->getActiveSequence().lock();
	auto sampler = mpc->getSampler();
	auto track = sequence->getTrack(sequencer->getActiveTrackIndex()).lock();
	auto lSampler = sampler.lock();
	auto bus = track->getBusNumber();
	auto programNumber = lSampler->getDrumBusProgramNumber(bus);
	auto program = lSampler->getProgram(programNumber);
	auto lSound = sound.lock();
	auto lProgram = program.lock();
	if (samplerGui->getNote() != 34) {
		auto noteParameters = dynamic_cast<mpc::sampler::NoteParameters*>(lProgram->getNoteParameters(samplerGui->getNote()));
		noteParameters->setSoundNumber(lSampler->getSoundCount() - 1);
		if (sound.lock()->isLoopEnabled()) {
			noteParameters->setVoiceOverlap(2);
		}

		auto pn = lProgram->getPadNumberFromNote(samplerGui->getNote());
		if (pn != -1) {
			auto pad = dynamic_pointer_cast<mpc::sampler::Program>(lProgram)->getPad(pn);
			auto mixerChannel = pad->getStereoMixerChannel().lock();
			if (lSound->isMono()) {
				mixerChannel->setStereo(false);
			}
			else {
				mixerChannel->setStereo(true);
			}
		}
	}
	soundGui->initZones(lSound->getLastFrameIndex());
}
