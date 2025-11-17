#pragma once

#include "IntTypes.hpp"
#include "sampler/VoiceOverlapMode.hpp"

#include <array>

namespace mpc::performance
{
    struct StereoMixer
    {
        DrumMixerLevel level{MaxDrumMixerLevel};
        DrumMixerPanning panning{PanningCenter};
    };

    struct IndivFxMixer
    {
        DrumMixerLevel individualOutLevel{MaxDrumMixerLevel};
        DrumMixerLevel fxSendLevel{MinDrumMixerLevel};
        DrumMixerIndividualOutput individualOutput{MinDrumMixerIndividualOut};
        DrumMixerIndividualFxPath fxPath{MinDrumMixerFxPath};
        bool followStereo = false;
    };

    struct NoteParameters
    {
        DrumNoteNumber noteNumber{NoDrumNoteAssigned};
        StereoMixer stereoMixer{};
        IndivFxMixer indivFxMixer{};
        DrumNoteNumber optionalNoteA{NoDrumNoteAssigned};
        DrumNoteNumber optionalNoteB{NoDrumNoteAssigned};
        sampler::VoiceOverlapMode voiceOverlapMode =
            sampler::VoiceOverlapMode::POLY;
        DrumNoteNumber muteAssignA{NoDrumNoteAssigned};
        DrumNoteNumber muteAssignB{NoDrumNoteAssigned};

        int16_t soundIndex = -1;

        int8_t soundGenerationMode = 0;
        int8_t velocityRangeLower = 44;
        int8_t velocityRangeUpper = 88;
        int8_t tune = 0;
        int8_t attack = 0;
        int8_t decay = 5;
        int8_t decayMode = 0;
        int8_t filterFrequency = 100;
        int8_t filterResonance = 0;
        int8_t filterAttack = 0;
        int8_t filterDecay = 0;
        int8_t filterEnvelopeAmount = 0;
        int8_t velocityToLevel = 100;
        int8_t velocityToAttack = 0;
        int8_t velocityToStart = 0;
        int8_t velocityToFilterFrequency = 0;
        int8_t sliderParameterNumber = 0;
        int8_t velocityToPitch = 0;
    };

    struct Pad
    {
        DrumNoteNumber note{NoDrumNoteAssigned};
        ProgramPadIndex index{0};
    };

    struct Slider
    {
        DrumNoteNumber assignNote{NoDrumNoteAssigned};
        int tuneLowRange = -120;
        int tuneHighRange = 120;
        int decayLowRange = 12;
        int decayHighRange = 45;
        int attackLowRange = 0;
        int attackHighRange = 20;
        int filterLowRange = -50;
        int filterHighRange = 50;
        int controlChange = 0;
        int parameter = 0;
    };

    struct Program
    {
        std::array<NoteParameters, 64> noteParameters{};
        std::array<Pad, 64> pads{};
        Slider slider{};
        int midiProgramChange = 0;

        Program()
        {
            for (int i = 0; i < 64; i++)
            {
                noteParameters[i].noteNumber =
                    DrumNoteNumber(i + MinDrumNoteNumber.get());
            }
        }
        NoteParameters getNoteParameters(const DrumNoteNumber noteNumber) const
        {
            return noteParameters[noteNumber.get() - MinDrumNoteNumber.get()];
        }
    };

    struct Drum
    {
        DrumBusIndex drumBusIndex;
        ProgramIndex programIndex;
        std::array<StereoMixer, 64> stereoMixers{};
        std::array<IndivFxMixer, 64> indivFxMixers{};
        std::array<std::pair<DrumNoteNumber, DrumNoteNumber>, 64>
            simultaneousNotesA{
                std::pair{NoDrumNoteAssigned, NoDrumNoteAssigned}};
        std::array<std::pair<DrumNoteNumber, DrumNoteNumber>, 64>
            simultaneousNotesB{
                std::pair{NoDrumNoteAssigned, NoDrumNoteAssigned}};
        bool receivePgmChange = false;
        bool receiveMidiVolume = false;
        MidiValue lastReceivedMidiVolume{MaxMidiValue};

        const StereoMixer &
        getStereoMixer(const DrumNoteNumber drumNoteNumber) const
        {
            return stereoMixers[drumNoteNumber.get() - MinDrumNoteNumber.get()];
        }

        const IndivFxMixer &
        getIndivFxMixer(const DrumNoteNumber drumNoteNumber) const
        {
            return indivFxMixers[drumNoteNumber.get() -
                                 MinDrumNoteNumber.get()];
        }
    };
} // namespace mpc::performance
