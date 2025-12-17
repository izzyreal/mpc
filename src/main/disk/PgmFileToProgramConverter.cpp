#include "disk/PgmFileToProgramConverter.hpp"

#include "Mpc.hpp"
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
#include <array>

#include "engine/IndivFxMixer.hpp"
#include "engine/StereoMixer.hpp"
#include "performance/PerformanceManager.hpp"

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
    mpc::Mpc &mpc, const ProgramFileReader &reader,
    const std::shared_ptr<Program> &program)
{
    const auto pgmNoteParameters = reader.getAllNoteParameters();
    const auto pgmPads = reader.getPads();

    std::array<performance::NoteParameters, Mpc2000XlSpecs::PROGRAM_PAD_COUNT>
        allPerfNoteParams;

    for (int programPadIndex = 0;
         programPadIndex < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; ++programPadIndex)
    {
        const auto padNote = pgmPads->getNote(programPadIndex);
        const auto note = padNote == -1 ? NoDrumNoteAssigned : padNote;
        program->getPad(programPadIndex)->setNote(DrumNoteNumber(note));

        auto &perfNoteParams = allPerfNoteParams[programPadIndex];

        perfNoteParams.attack = pgmNoteParameters->getAttack(programPadIndex);
        perfNoteParams.decay = pgmNoteParameters->getDecay(programPadIndex);
        perfNoteParams.decayMode =
            pgmNoteParameters->getDecayMode(programPadIndex);
        perfNoteParams.filterAttack =
            pgmNoteParameters->getVelEnvToFiltAtt(programPadIndex);
        perfNoteParams.filterDecay =
            pgmNoteParameters->getVelEnvToFiltDec(programPadIndex);
        perfNoteParams.filterEnvelopeAmount =
            pgmNoteParameters->getVelEnvToFiltAmt(programPadIndex);
        perfNoteParams.filterFrequency =
            pgmNoteParameters->getCutoff(programPadIndex);
        perfNoteParams.filterResonance =
            pgmNoteParameters->getResonance(programPadIndex);
        perfNoteParams.muteAssignA =
            DrumNoteNumber(pgmNoteParameters->getMuteAssign1(programPadIndex));
        perfNoteParams.muteAssignB =
            DrumNoteNumber(pgmNoteParameters->getMuteAssign2(programPadIndex));
        perfNoteParams.optionalNoteA =
            DrumNoteNumber(pgmNoteParameters->getAlsoPlayUse1(programPadIndex));
        perfNoteParams.optionalNoteB =
            DrumNoteNumber(pgmNoteParameters->getAlsoPlayUse2(programPadIndex));

        const auto sampleSelect =
            pgmNoteParameters->getSampleSelect(programPadIndex);

        perfNoteParams.soundIndex = sampleSelect == 255 ? -1 : sampleSelect;

        perfNoteParams.sliderParameterNumber =
            pgmNoteParameters->getSliderParameter(programPadIndex);
        perfNoteParams.soundGenerationMode =
            pgmNoteParameters->getSoundGenerationMode(programPadIndex);
        perfNoteParams.tune = pgmNoteParameters->getTune(programPadIndex);
        perfNoteParams.velocityRangeLower =
            pgmNoteParameters->getVelocityRangeLower(programPadIndex);
        perfNoteParams.velocityRangeUpper =
            pgmNoteParameters->getVelocityRangeUpper(programPadIndex);
        perfNoteParams.velocityToAttack =
            pgmNoteParameters->getVelocityToAttack(programPadIndex);
        perfNoteParams.velocityToFilterFrequency =
            pgmNoteParameters->getVelocityToCutoff(programPadIndex);
        perfNoteParams.velocityToLevel =
            pgmNoteParameters->getVelocityToLevel(programPadIndex);
        perfNoteParams.velocityToPitch =
            pgmNoteParameters->getVelocityToPitch(programPadIndex);
        perfNoteParams.velocityToStart =
            pgmNoteParameters->getVelocityToStart(programPadIndex);
        perfNoteParams.voiceOverlapMode =
            pgmNoteParameters->getVoiceOverlapMode(programPadIndex);
    }

    performance::UpdateAllNoteParametersBulk msg{program->getProgramIndex(),
                                                 allPerfNoteParams};
    mpc.getPerformanceManager().lock()->enqueue(msg);
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
    mpc::Mpc &mpc, const std::shared_ptr<MpcFile> &f,
    std::shared_ptr<Program> program, std::vector<std::string> &soundNames)
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

    setNoteParameters(mpc, reader, program);
    setMixer(reader, program);
    setSlider(reader, program);

    return program;
}
