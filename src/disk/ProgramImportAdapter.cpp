#include <disk/ProgramImportAdapter.hpp>

#include <Mpc.hpp>
#include <sampler/StereoMixerChannel.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::disk;
using namespace std;

ProgramImportAdapter::ProgramImportAdapter(weak_ptr<mpc::sampler::Sampler> sampler, weak_ptr<mpc::sampler::Program> inputProgram, vector<int> soundsDestIndex)
{
	this->sampler = sampler;
	this->soundsDestIndex = soundsDestIndex;
	result = inputProgram;
	auto lResult = result.lock();
	for (int i = 35; i <= 98; i++) {
		processNoteParameters(lResult->getNoteParameters(i));
		initMixer(i);
	}
}

void ProgramImportAdapter::processNoteParameters(mpc::sampler::NoteParameters* np)
{
	auto const pgmSoundNumber = np->getSndNumber();
	if (pgmSoundNumber == -1)
		return;
	np->setSoundNumber(soundsDestIndex[pgmSoundNumber]);
}

void ProgramImportAdapter::initMixer(int note)
{
	auto lResult = result.lock();
	auto sound = sampler.lock()->getSound(lResult->getNoteParameters(note)->getSndNumber()).lock();
	if (!sound) return;

	auto pad = lResult->getPad(lResult->getPadNumberFromNote(note));
	auto mc = pad->getStereoMixerChannel().lock();
	if (sound->isMono()) {
		mc->setStereo(false);
	}
	else {
		mc->setStereo(true);
	}
}

weak_ptr<mpc::sampler::Program> ProgramImportAdapter::get()
{
    return result;
}
