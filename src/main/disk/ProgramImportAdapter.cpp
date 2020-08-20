#include <disk/ProgramImportAdapter.hpp>

#include <Mpc.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::disk;
using namespace mpc::sampler;
using namespace std;

ProgramImportAdapter::ProgramImportAdapter(weak_ptr<mpc::sampler::Sampler> sampler, weak_ptr<mpc::sampler::Program> inputProgram, vector<int> soundsDestIndex)
{
	this->sampler = sampler;
	this->soundsDestIndex = soundsDestIndex;
	result = inputProgram;
	auto lResult = result.lock();
	for (int i = 35; i <= 98; i++) {
		processNoteParameters(dynamic_cast<mpc::sampler::NoteParameters*>(lResult->getNoteParameters(i)));
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
	auto noteParameters = dynamic_cast<NoteParameters*>(lResult->getNoteParameters(note));
	auto sound = sampler.lock()->getSound(noteParameters->getSndNumber()).lock();

	if (!sound)
		return;

	auto mc = noteParameters->getStereoMixerChannel().lock();
	mc->setStereo(!sound->isMono());
}

weak_ptr<mpc::sampler::Program> ProgramImportAdapter::get()
{
    return result;
}
