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

ProgramImportAdapter::ProgramImportAdapter(
	weak_ptr<Sampler> sampler,
	weak_ptr<Program> inputProgram,
	vector<int> soundsDestIndex,
	vector<int> unavailableSoundIndices
)
{
	this->sampler = sampler;
	this->soundsDestIndex = soundsDestIndex;
	result = inputProgram;
	auto lResult = result.lock();

	for (int i = 35; i <= 98; i++)
	{
		auto noteParameters = dynamic_cast<NoteParameters*>(lResult->getNoteParameters(i));
		
		if (find(begin(unavailableSoundIndices), end(unavailableSoundIndices), noteParameters->getSoundIndex()) != end(unavailableSoundIndices))
			noteParameters->setSoundIndex(-1);

		processNoteParameters(noteParameters);
		initMixer(i);
	}
}

void ProgramImportAdapter::processNoteParameters(NoteParameters* np)
{
	auto const pgmSoundNumber = np->getSoundIndex();
	
	if (pgmSoundNumber == -1)
		return;

	if (soundsDestIndex[pgmSoundNumber] >= sampler.lock()->getSoundCount())
	{
		np->setSoundIndex(-1);
	}
	else
	{
		np->setSoundIndex(soundsDestIndex[pgmSoundNumber]);
	}
}

void ProgramImportAdapter::initMixer(int note)
{
	auto lResult = result.lock();
	auto noteParameters = dynamic_cast<NoteParameters*>(lResult->getNoteParameters(note));
	auto sound = sampler.lock()->getSound(noteParameters->getSoundIndex()).lock();

	if (!sound)
		return;

	auto mc = noteParameters->getStereoMixerChannel().lock();
	mc->setStereo(!sound->isMono());
}

weak_ptr<Program> ProgramImportAdapter::get()
{
    return result;
}
