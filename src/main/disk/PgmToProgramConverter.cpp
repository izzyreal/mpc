#include <disk/PgmToProgramConverter.hpp>

#include <file/pgmreader/PRMixer.hpp>
#include <file/pgmreader/PRPads.hpp>
#include <file/pgmreader/PgmAllNoteParameters.hpp>
#include <file/pgmreader/PgmHeader.hpp>
#include <file/pgmreader/ProgramFileReader.hpp>
#include <file/pgmreader/ProgramName.hpp>
#include <file/pgmreader/PRSlider.hpp>
#include <file/pgmreader/SoundNames.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/PgmSlider.hpp>
#include <sampler/Sampler.hpp>

#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcNoteParameters.hpp>

using namespace mpc::disk;
using namespace mpc::sampler;
using namespace std;

PgmToProgramConverter::PgmToProgramConverter(MpcFile* file, weak_ptr<Sampler> sampler, const int replaceIndex)
{
	if (replaceIndex == -1)
		program = sampler.lock()->addProgram();
	else
		program = dynamic_pointer_cast<Program>(sampler.lock()->getProgram(replaceIndex).lock());

	reader = new mpc::file::pgmreader::ProgramFileReader(file);

	auto pgmSoundNames = reader->getSampleNames();
	
	for (int i = 0; i < reader->getHeader()->getNumberOfSamples(); i++)
		soundNames.push_back(pgmSoundNames->getSampleName(i));

	auto const programName = reader->getProgramName();
	program.lock()->setName(programName->getProgramNameASCII());
	setNoteParameters();
	setMixer();
	setSlider();
	done = true;
}

PgmToProgramConverter::~PgmToProgramConverter()
{
	if (reader != nullptr)
		delete reader;
}

void PgmToProgramConverter::setSlider()
{
	auto slider = reader->getSlider();
	auto nn = slider->getMidiNoteAssign() == 0 ? 34 : slider->getMidiNoteAssign();
	auto pgmSlider = dynamic_cast<PgmSlider*>(program.lock()->getSlider());
	pgmSlider->setAssignNote(nn);
	pgmSlider->setAttackHighRange(slider->getAttackHigh());
	pgmSlider->setAttackLowRange(slider->getAttackLow());
	pgmSlider->setControlChange(slider->getControlChange());
	pgmSlider->setDecayHighRange(slider->getDecayHigh());
	pgmSlider->setDecayLowRange(slider->getDecayLow());
	pgmSlider->setFilterHighRange(slider->getFilterHigh());
	pgmSlider->setFilterLowRange(slider->getFilterLow());
	pgmSlider->setTuneHighRange(slider->getTuneHigh());
	pgmSlider->setTuneLowRange(slider->getTuneLow());
}

void PgmToProgramConverter::setNoteParameters()
{
	auto pgmNoteParameters = reader->getAllNoteParameters();
	auto pgmPads = reader->getPads();
	int pmn = 0;
	int nn = 0;
	NoteParameters* programNoteParameters = nullptr;
	auto lProgram = program.lock();
	for (int i = 0; i < 64; i++) {
		pmn = pgmPads->getNote(i);
		nn = pmn == -1 ? 34 : pmn;
		lProgram->getPad(i)->setNote(nn);
		programNoteParameters = dynamic_cast<NoteParameters*>(lProgram->getNoteParameters(i + 35));
		programNoteParameters->setAttack(pgmNoteParameters->getAttack(i));
		programNoteParameters->setDecay(pgmNoteParameters->getDecay(i));
		programNoteParameters->setDecayMode(pgmNoteParameters->getDecayMode(i));
		programNoteParameters->setFilterAttack(pgmNoteParameters->getVelEnvToFiltAtt(i));
		programNoteParameters->setFilterDecay(pgmNoteParameters->getVelEnvToFiltDec(i));
		programNoteParameters->setFilterEnvelopeAmount(pgmNoteParameters->getVelEnvToFiltAmt(i));
		programNoteParameters->setFilterFrequency(pgmNoteParameters->getCutoff(i));
		programNoteParameters->setFilterResonance(pgmNoteParameters->getResonance(i));
		programNoteParameters->setMuteAssignA(pgmNoteParameters->getMuteAssign1(i));
		programNoteParameters->setMuteAssignB(pgmNoteParameters->getMuteAssign2(i));
		programNoteParameters->setOptNoteA(pgmNoteParameters->getAlsoPlayUse1(i));
		programNoteParameters->setOptionalNoteB(pgmNoteParameters->getAlsoPlayUse2(i));
		auto sampleSelect = pgmNoteParameters->getSampleSelect(i);
		programNoteParameters->setSoundNumberNoLimit(sampleSelect == 255 ? -1 : sampleSelect);
		programNoteParameters->setSliderParameterNumber(pgmNoteParameters->getSliderParameter(i));
		programNoteParameters->setSoundGenMode(pgmNoteParameters->getSoundGenerationMode(i));
		programNoteParameters->setTune(pgmNoteParameters->getTune(i));
		programNoteParameters->setVeloRangeLower(pgmNoteParameters->getVelocityRangeLower(i));
		programNoteParameters->setVeloRangeUpper(pgmNoteParameters->getVelocityRangeUpper(i));
		programNoteParameters->setVelocityToAttack(pgmNoteParameters->getVelocityToAttack(i));
		programNoteParameters->setVelocityToFilterFrequency(pgmNoteParameters->getVelocityToCutoff(i));
		programNoteParameters->setVeloToLevel(pgmNoteParameters->getVelocityToLevel(i));
		programNoteParameters->setVelocityToPitch(pgmNoteParameters->getVelocityToPitch(i));
		programNoteParameters->setVelocityToStart(pgmNoteParameters->getVelocityToStart(i));
		programNoteParameters->setVoiceOverlap(pgmNoteParameters->getVoiceOverlap(i));
	}
}

void PgmToProgramConverter::setMixer()
{
	auto pgmMixer = reader->getMixer();
	auto pgmPads = reader->getPads();
	auto skippedMixerChannels = 0;
	auto lProgram = program.lock();
	for (int i = 0; i < 64; i++) {
		auto pmn = pgmPads->getNote(i);
		if (pmn == -1) {
			lProgram->getPad(i)->setNote(34);
		}
		else {
			lProgram->getPad(i)->setNote(pmn);
		}
		if (pmn != -1) {
			auto mixindex = pmn - 35 - skippedMixerChannels;
			auto smc = lProgram->getPad(i)->getStereoMixerChannel().lock();
			auto ifmc = lProgram->getPad(i)->getIndivFxMixerChannel().lock();
			smc->setLevel(pgmMixer->getVolume(mixindex));
			smc->setPanning(pgmMixer->getPan(mixindex));
			ifmc->setVolumeIndividualOut(pgmMixer->getVolumeIndividual(mixindex));
			ifmc->setOutput(pgmMixer->getOutput(mixindex));
			ifmc->setFxPath(pgmMixer->getEffectsOutput(mixindex));
		}
		else {
			skippedMixerChannels++;
		}
	}
}

weak_ptr<Program> PgmToProgramConverter::get()
{
	if (!done) return weak_ptr<Program>();
	return program;
}

vector<string> PgmToProgramConverter::getSoundNames()
{
	if (!done) return vector<string>(0);
    return soundNames;
}
