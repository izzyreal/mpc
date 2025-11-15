#include "performance/ProgramMapper.hpp"

#include "IntTypes.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Program.hpp"
#include "performance/Drum.hpp"

using namespace mpc;

namespace mpc::performance
{
    void mapSamplerProgramToPerformanceProgram(const mpc::sampler::Program &src,
                                               mpc::performance::Program &dst)
    {
        for (int i = 0; i < 64; ++i)
        {
            auto *sn = src.getNoteParameters(i + MinDrumNoteNumber);
            auto &dn = dst.noteParameters[i];

            dn.noteNumber = sn->getNumber();

            const auto stereo = sn->getStereoMixerChannel();
            dn.stereoMixer.level = DrumMixerLevel(stereo->getLevel());
            dn.stereoMixer.panning = DrumMixerPanning(stereo->getPanning());

            const auto fx = sn->getIndivFxMixerChannel();
            dn.indivFxMixer.individualOutLevel =
                DrumMixerLevel(fx->getVolumeIndividualOut());
            dn.indivFxMixer.fxSendLevel = DrumMixerLevel(fx->getFxSendLevel());
            dn.indivFxMixer.individualOutput =
                DrumMixerIndividualOutput(fx->getOutput());
            dn.indivFxMixer.fxPath = DrumMixerIndividualFxPath(fx->getFxPath());
            dn.indivFxMixer.followStereo = fx->isFollowingStereo();

            dn.soundIndex = sn->getSoundIndex();
            dn.soundGenerationMode = sn->getSoundGenerationMode();
            dn.velocityRangeLower = sn->getVelocityRangeLower();
            dn.optionalNoteA = sn->getOptionalNoteA();
            dn.velocityRangeUpper = sn->getVelocityRangeUpper();
            dn.optionalNoteB = sn->getOptionalNoteB();
            dn.voiceOverlapMode =
                static_cast<int8_t>(sn->getVoiceOverlapMode());
            dn.muteAssignA = sn->getMuteAssignA();
            dn.muteAssignB = sn->getMuteAssignB();
            dn.tune = sn->getTune();
            dn.attack = sn->getAttack();
            dn.decay = sn->getDecay();
            dn.decayMode = sn->getDecayMode();
            dn.filterFrequency = sn->getFilterFrequency();
            dn.filterResonance = sn->getFilterResonance();
            dn.filterAttack = sn->getFilterAttack();
            dn.filterDecay = sn->getFilterDecay();
            dn.filterEnvelopeAmount = sn->getFilterEnvelopeAmount();
            dn.velocityToLevel = sn->getVeloToLevel();
            dn.velocityToAttack = sn->getVelocityToAttack();
            dn.velocityToStart = sn->getVelocityToStart();
            dn.velocityToFilterFrequency = sn->getVelocityToFilterFrequency();
            dn.sliderParameterNumber = sn->getSliderParameterNumber();
            dn.velocityToPitch = sn->getVelocityToPitch();
        }

        for (int i = 0; i < 64; ++i)
        {
            const auto *sp = src.getPad(i);
            auto &dp = dst.pads[i];

            dp.note = sp->getNote();
            dp.index = sp->getIndex();
        }

        {
            const auto &ss = src.getSlider();
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

        dst.midiProgramChange = src.getMidiProgramChange();
    }
} // namespace mpc::performance