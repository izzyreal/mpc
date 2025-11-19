#include "performance/ProgramMapper.hpp"

#include "IntTypes.hpp"
#include "engine/IndivFxMixer.hpp"
#include "engine/StereoMixer.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Program.hpp"
#include "performance/Drum.hpp"

using namespace mpc;

namespace mpc::performance
{
    NoteParameters mapSamplerNoteParametersToPerformanceNoteParameters(
        const sampler::NoteParameters *s)
    {
        NoteParameters result;
        mapSamplerNoteParametersToPerformanceNoteParameters(s, result);
        return result;
    }

    void mapSamplerNoteParametersToPerformanceNoteParameters(
        const sampler::NoteParameters *s, NoteParameters &p)
    {
        p.noteNumber = s->getNumber();
        p.soundIndex = s->getSoundIndex();
        p.voiceOverlapMode = s->getVoiceOverlapMode();
        p.muteAssignA = s->getMuteAssignA();
        p.muteAssignB = s->getMuteAssignB();
        p.tune = s->getTune();
        p.attack = s->getAttack();
        p.decay = s->getDecay();
        p.decayMode = s->getDecayMode();
        p.filterFrequency = s->getFilterFrequency();
        p.filterResonance = s->getFilterResonance();
        p.filterEnvelopeAmount = s->getFilterEnvelopeAmount();
        p.filterEnvelopeAmount = s->getFilterAttack();
        p.filterDecay = s->getFilterDecay();
        p.velocityRangeLower = s->getVelocityRangeLower();
        p.velocityRangeUpper = s->getVelocityRangeUpper();
        p.velocityToFilterFrequency = s->getVelocityToFilterFrequency();
        p.velocityToAttack = s->getVelocityToAttack();
        p.velocityToLevel = s->getVeloToLevel();
        p.velocityToPitch = s->getVelocityToPitch();
        p.velocityToStart = s->getVelocityToStart();
        p.optionalNoteA = s->getOptionalNoteA();
        p.optionalNoteB = s->getOptionalNoteB();
        p.sliderParameterNumber = s->getSliderParameterNumber();
        p.soundGenerationMode = s->getSoundGenerationMode();

        p.stereoMixer.level = s->getStereoMixer()->getLevel();
        p.stereoMixer.panning = s->getStereoMixer()->getPanning();

        p.indivFxMixer.followStereo = s->getIndivFxMixer()->isFollowingStereo();
        p.indivFxMixer.individualOutput = s->getIndivFxMixer()->getOutput();
        p.indivFxMixer.individualOutLevel =
            s->getIndivFxMixer()->getVolumeIndividualOut();
        p.indivFxMixer.fxPath = s->getIndivFxMixer()->getFxPath();
        p.indivFxMixer.fxSendLevel = s->getIndivFxMixer()->getFxSendLevel();
    }

    void mapSamplerProgramToPerformanceProgram(
        const sampler::Program &samplerProgram, Program &dst)
    {
        for (int i = 0; i < 64; ++i)
        {
            const auto pgmNoteParameters =
                samplerProgram.getNoteParameters(i + MinDrumNoteNumber);
            auto &perfNoteParameters = dst.noteParameters[i];

            perfNoteParameters.noteNumber = pgmNoteParameters->getNumber();

            const auto stereo = pgmNoteParameters->getStereoMixer();
            perfNoteParameters.stereoMixer.level =
                DrumMixerLevel(stereo->getLevel());
            perfNoteParameters.stereoMixer.panning =
                DrumMixerPanning(stereo->getPanning());

            const auto fx = pgmNoteParameters->getIndivFxMixer();
            perfNoteParameters.indivFxMixer.individualOutLevel =
                DrumMixerLevel(fx->getVolumeIndividualOut());
            perfNoteParameters.indivFxMixer.fxSendLevel =
                DrumMixerLevel(fx->getFxSendLevel());
            perfNoteParameters.indivFxMixer.individualOutput =
                DrumMixerIndividualOutput(fx->getOutput());
            perfNoteParameters.indivFxMixer.fxPath =
                DrumMixerIndividualFxPath(fx->getFxPath());
            perfNoteParameters.indivFxMixer.followStereo =
                fx->isFollowingStereo();

            perfNoteParameters.soundIndex = pgmNoteParameters->getSoundIndex();
            perfNoteParameters.soundGenerationMode =
                pgmNoteParameters->getSoundGenerationMode();
            perfNoteParameters.velocityRangeLower =
                pgmNoteParameters->getVelocityRangeLower();
            perfNoteParameters.optionalNoteA =
                pgmNoteParameters->getOptionalNoteA();
            perfNoteParameters.velocityRangeUpper =
                pgmNoteParameters->getVelocityRangeUpper();
            perfNoteParameters.optionalNoteB =
                pgmNoteParameters->getOptionalNoteB();
            perfNoteParameters.voiceOverlapMode =
                pgmNoteParameters->getVoiceOverlapMode();
            perfNoteParameters.muteAssignA =
                pgmNoteParameters->getMuteAssignA();
            perfNoteParameters.muteAssignB =
                pgmNoteParameters->getMuteAssignB();
            perfNoteParameters.tune = pgmNoteParameters->getTune();
            perfNoteParameters.attack = pgmNoteParameters->getAttack();
            perfNoteParameters.decay = pgmNoteParameters->getDecay();
            perfNoteParameters.decayMode = pgmNoteParameters->getDecayMode();
            perfNoteParameters.filterFrequency =
                pgmNoteParameters->getFilterFrequency();
            perfNoteParameters.filterResonance =
                pgmNoteParameters->getFilterResonance();
            perfNoteParameters.filterAttack =
                pgmNoteParameters->getFilterAttack();
            perfNoteParameters.filterDecay =
                pgmNoteParameters->getFilterDecay();
            perfNoteParameters.filterEnvelopeAmount =
                pgmNoteParameters->getFilterEnvelopeAmount();
            perfNoteParameters.velocityToLevel =
                pgmNoteParameters->getVeloToLevel();
            perfNoteParameters.velocityToAttack =
                pgmNoteParameters->getVelocityToAttack();
            perfNoteParameters.velocityToStart =
                pgmNoteParameters->getVelocityToStart();
            perfNoteParameters.velocityToFilterFrequency =
                pgmNoteParameters->getVelocityToFilterFrequency();
            perfNoteParameters.sliderParameterNumber =
                pgmNoteParameters->getSliderParameterNumber();
            perfNoteParameters.velocityToPitch =
                pgmNoteParameters->getVelocityToPitch();
        }

        for (int i = 0; i < 64; ++i)
        {
            const auto *sp = samplerProgram.getPad(i);
            auto &dp = dst.pads[i];

            dp.note = sp->getNote();
            dp.index = sp->getIndex();
        }

        {
            const auto &ss = samplerProgram.getSlider();
            auto &ds = dst.slider;

            ds.assignNote = ss->getNote();
            ds.tuneLowRange = ss->getTuneLowRange();
            ds.tuneHighRange = ss->getTuneHighRange();
            ds.decayLowRange = ss->getDecayLowRange();
            ds.decayHighRange = ss->getDecayHighRange();
            ds.attackLowRange = ss->getAttackLowRange();
            ds.attackHighRange = ss->getAttackHighRange();
            ds.filterLowRange = ss->getFilterLowRange();
            ds.filterHighRange = ss->getFilterHighRange();
            ds.controlChange = ss->getControlChange();
            ds.parameter = ss->getParameter();
        }

        dst.midiProgramChange = samplerProgram.getMidiProgramChange();
    }
} // namespace mpc::performance