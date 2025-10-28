#include "disk/PgmFileToProgramConverter.hpp"

#include "file/pgmreader/PRMixer.hpp"
#include "file/pgmreader/PRPads.hpp"
#include "file/pgmreader/PgmAllNoteParameters.hpp"
#include "file/pgmreader/PgmHeader.hpp"
#include "file/pgmreader/ProgramFileReader.hpp"
#include "file/pgmreader/ProgramName.hpp"
#include "file/pgmreader/PRSlider.hpp"
#include "file/pgmreader/SoundNames.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Program.hpp"
#include "sampler/PgmSlider.hpp"
#include "sampler/NoteParameters.hpp"

#include "disk/MpcFile.hpp"

#include "StrUtil.hpp"

#include <stdexcept>

using namespace mpc::disk;
using namespace mpc::sampler;
using namespace mpc::file::pgmreader;

void PgmFileToProgramConverter::setSlider(ProgramFileReader &reader,
                                          std::shared_ptr<Program> program)
{
    auto slider = reader.getSlider();
    auto nn =
        slider->getMidiNoteAssign() == 0 ? 34 : slider->getMidiNoteAssign();
    auto pgmSlider = program->getSlider();
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

void PgmFileToProgramConverter::setNoteParameters(
    ProgramFileReader &reader, std::shared_ptr<Program> program)
{
    auto pgmNoteParameters = reader.getAllNoteParameters();
    auto pgmPads = reader.getPads();
    NoteParameters *programNoteParameters;

    for (int i = 0; i < 64; i++)
    {
        auto padNote = pgmPads->getNote(i);
        auto note = padNote == -1 ? 34 : padNote;
        program->getPad(i)->setNote(note);

        programNoteParameters =
            dynamic_cast<NoteParameters *>(program->getNoteParameters(i + 35));
        programNoteParameters->setAttack(pgmNoteParameters->getAttack(i));
        programNoteParameters->setDecay(pgmNoteParameters->getDecay(i));
        programNoteParameters->setDecayMode(pgmNoteParameters->getDecayMode(i));
        programNoteParameters->setFilterAttack(
            pgmNoteParameters->getVelEnvToFiltAtt(i));
        programNoteParameters->setFilterDecay(
            pgmNoteParameters->getVelEnvToFiltDec(i));
        programNoteParameters->setFilterEnvelopeAmount(
            pgmNoteParameters->getVelEnvToFiltAmt(i));
        programNoteParameters->setFilterFrequency(
            pgmNoteParameters->getCutoff(i));
        programNoteParameters->setFilterResonance(
            pgmNoteParameters->getResonance(i));
        programNoteParameters->setMuteAssignA(
            pgmNoteParameters->getMuteAssign1(i));
        programNoteParameters->setMuteAssignB(
            pgmNoteParameters->getMuteAssign2(i));
        programNoteParameters->setOptNoteA(
            pgmNoteParameters->getAlsoPlayUse1(i));
        programNoteParameters->setOptionalNoteB(
            pgmNoteParameters->getAlsoPlayUse2(i));

        auto sampleSelect = pgmNoteParameters->getSampleSelect(i);
        programNoteParameters->setSoundIndex(
            sampleSelect == 255 ? -1 : sampleSelect);
        programNoteParameters->setSliderParameterNumber(
            pgmNoteParameters->getSliderParameter(i));
        programNoteParameters->setSoundGenMode(
            pgmNoteParameters->getSoundGenerationMode(i));
        programNoteParameters->setTune(pgmNoteParameters->getTune(i));
        programNoteParameters->setVeloRangeLower(
            pgmNoteParameters->getVelocityRangeLower(i));
        programNoteParameters->setVeloRangeUpper(
            pgmNoteParameters->getVelocityRangeUpper(i));
        programNoteParameters->setVelocityToAttack(
            pgmNoteParameters->getVelocityToAttack(i));
        programNoteParameters->setVelocityToFilterFrequency(
            pgmNoteParameters->getVelocityToCutoff(i));
        programNoteParameters->setVeloToLevel(
            pgmNoteParameters->getVelocityToLevel(i));
        programNoteParameters->setVelocityToPitch(
            pgmNoteParameters->getVelocityToPitch(i));
        programNoteParameters->setVelocityToStart(
            pgmNoteParameters->getVelocityToStart(i));
        programNoteParameters->setVoiceOverlapMode(
            pgmNoteParameters->getVoiceOverlapMode(i));
    }
}

void PgmFileToProgramConverter::setMixer(ProgramFileReader &reader,
                                         std::shared_ptr<Program> program)
{
    auto pgmMixer = reader.getMixer();

    for (int i = 0; i < 64; i++)
    {
        auto noteParameters =
            dynamic_cast<NoteParameters *>(program->getNoteParameters(i + 35));
        auto smc = noteParameters->getStereoMixerChannel();
        auto ifmc = noteParameters->getIndivFxMixerChannel();

        smc->setLevel(pgmMixer->getVolume(i));
        smc->setPanning(pgmMixer->getPan(i));
        ifmc->setVolumeIndividualOut(pgmMixer->getVolumeIndividual(i));
        ifmc->setOutput(pgmMixer->getOutput(i));
        ifmc->setFxPath(pgmMixer->getEffectsOutput(i));
    }
}

program_or_error PgmFileToProgramConverter::loadFromFileAndConvert(
    std::shared_ptr<MpcFile> f, std::shared_ptr<mpc::sampler::Program> program,
    std::vector<std::string> &soundNames)
{
    if (!f->exists())
    {
        throw std::invalid_argument("File does not exist");
    }

    ProgramFileReader reader(f);

    if (!reader.getHeader()->verifyMagic())
    {
        throw std::invalid_argument("PGM first 2 bytes are incorrect");
    }

    auto pgmSoundNames = reader.getSampleNames();

    for (int i = 0; i < reader.getHeader()->getSoundCount(); i++)
    {
        soundNames.push_back(pgmSoundNames->getSampleName(i));
    }

    const std::string programNameInData =
        StrUtil::trim(reader.getProgramName()->getProgramNameASCII());

    if (StrUtil::eqIgnoreCase(programNameInData, f->getNameWithoutExtension()))
    {
        program->setName(programNameInData);
    }
    else
    {
        program->setName(f->getNameWithoutExtension());
    }

    setNoteParameters(reader, program);
    setMixer(reader, program);
    setSlider(reader, program);

    return program;
}
