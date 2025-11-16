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

#include "engine/IndivFxMixer.hpp"
#include "engine/StereoMixer.hpp"

using namespace mpc::disk;
using namespace mpc::sampler;
using namespace mpc::file::pgmreader;

void PgmFileToProgramConverter::setSlider(
    const ProgramFileReader &reader, const std::shared_ptr<Program> &program)
{
    const auto slider = reader.getSlider();
    const auto nn = slider->getMidiNoteAssign() == 0
                        ? NoDrumNoteAssigned
                        : DrumNoteNumber(slider->getMidiNoteAssign());
    const auto pgmSlider = program->getSlider();
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
    const ProgramFileReader &reader, const std::shared_ptr<Program> &program)
{
    const auto pgmNoteParameters = reader.getAllNoteParameters();
    const auto pgmPads = reader.getPads();

    for (int programPadIndex = 0;
         programPadIndex < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; ++programPadIndex)
    {
        const auto padNote = pgmPads->getNote(programPadIndex);
        const auto note = padNote == -1 ? NoDrumNoteAssigned : padNote;
        program->getPad(programPadIndex)->setNote(DrumNoteNumber(note));

        NoteParameters *programNoteParameters =
            program->getNoteParameters(programPadIndex + MinDrumNoteNumber);
        programNoteParameters->setAttack(
            pgmNoteParameters->getAttack(programPadIndex));
        programNoteParameters->setDecay(
            pgmNoteParameters->getDecay(programPadIndex));
        programNoteParameters->setDecayMode(
            pgmNoteParameters->getDecayMode(programPadIndex));
        programNoteParameters->setFilterAttack(
            pgmNoteParameters->getVelEnvToFiltAtt(programPadIndex));
        programNoteParameters->setFilterDecay(
            pgmNoteParameters->getVelEnvToFiltDec(programPadIndex));
        programNoteParameters->setFilterEnvelopeAmount(
            pgmNoteParameters->getVelEnvToFiltAmt(programPadIndex));
        programNoteParameters->setFilterFrequency(
            pgmNoteParameters->getCutoff(programPadIndex));
        programNoteParameters->setFilterResonance(
            pgmNoteParameters->getResonance(programPadIndex));
        programNoteParameters->setMuteAssignA(
            DrumNoteNumber(pgmNoteParameters->getMuteAssign1(programPadIndex)));
        programNoteParameters->setMuteAssignB(
            DrumNoteNumber(pgmNoteParameters->getMuteAssign2(programPadIndex)));
        programNoteParameters->setOptionalNoteA(DrumNoteNumber(
            pgmNoteParameters->getAlsoPlayUse1(programPadIndex)));
        programNoteParameters->setOptionalNoteB(DrumNoteNumber(
            pgmNoteParameters->getAlsoPlayUse2(programPadIndex)));

        const auto sampleSelect =
            pgmNoteParameters->getSampleSelect(programPadIndex);
        programNoteParameters->setSoundIndex(
            sampleSelect == 255 ? -1 : sampleSelect);
        programNoteParameters->setSliderParameterNumber(
            pgmNoteParameters->getSliderParameter(programPadIndex));
        programNoteParameters->setSoundGenMode(
            pgmNoteParameters->getSoundGenerationMode(programPadIndex));
        programNoteParameters->setTune(
            pgmNoteParameters->getTune(programPadIndex));
        programNoteParameters->setVeloRangeLower(
            pgmNoteParameters->getVelocityRangeLower(programPadIndex));
        programNoteParameters->setVeloRangeUpper(
            pgmNoteParameters->getVelocityRangeUpper(programPadIndex));
        programNoteParameters->setVelocityToAttack(
            pgmNoteParameters->getVelocityToAttack(programPadIndex));
        programNoteParameters->setVelocityToFilterFrequency(
            pgmNoteParameters->getVelocityToCutoff(programPadIndex));
        programNoteParameters->setVeloToLevel(
            pgmNoteParameters->getVelocityToLevel(programPadIndex));
        programNoteParameters->setVelocityToPitch(
            pgmNoteParameters->getVelocityToPitch(programPadIndex));
        programNoteParameters->setVelocityToStart(
            pgmNoteParameters->getVelocityToStart(programPadIndex));
        programNoteParameters->setVoiceOverlapMode(
            pgmNoteParameters->getVoiceOverlapMode(programPadIndex));
    }
}

void PgmFileToProgramConverter::setMixer(
    const ProgramFileReader &reader, const std::shared_ptr<Program> &program)
{
    const auto pgmMixer = reader.getMixer();

    for (int i = 0; i < 64; i++)
    {
        const auto noteParameters = program->getNoteParameters(i + 35);
        const auto stereoMixer = noteParameters->getStereoMixer();
        const auto indivFxMixer = noteParameters->getIndivFxMixer();

        stereoMixer->setLevel(DrumMixerLevel(pgmMixer->getVolume(i)));
        stereoMixer->setPanning(DrumMixerPanning(pgmMixer->getPan(i)));
        indivFxMixer->setVolumeIndividualOut(
            DrumMixerLevel(pgmMixer->getVolumeIndividual(i)));
        indivFxMixer->setOutput(
            DrumMixerIndividualOutput(pgmMixer->getOutput(i)));
        indivFxMixer->setFxPath(
            DrumMixerIndividualFxPath(pgmMixer->getEffectsOutput(i)));
    }
}

program_or_error PgmFileToProgramConverter::loadFromFileAndConvert(
    const std::shared_ptr<MpcFile> &f, std::shared_ptr<Program> program,
    std::vector<std::string> &soundNames)
{
    if (!f->exists())
    {
        throw std::invalid_argument("File does not exist");
    }

    const ProgramFileReader reader(f);

    if (!reader.getHeader()->verifyMagic())
    {
        throw std::invalid_argument("PGM first 2 bytes are incorrect");
    }

    const auto pgmSoundNames = reader.getSampleNames();

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
