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
        int soundIndex = -1;
        int soundGenerationMode = 0;
        int velocityRangeLower = 44;
        int velocityRangeUpper = 88;
        DrumNoteNumber optionalNoteA{NoDrumNoteAssigned};
        DrumNoteNumber optionalNoteB{NoDrumNoteAssigned};
        sampler::VoiceOverlapMode voiceOverlapMode =
            sampler::VoiceOverlapMode::POLY;
        DrumNoteNumber muteAssignA{NoDrumNoteAssigned};
        DrumNoteNumber muteAssignB{NoDrumNoteAssigned};
        int tune = 0;
        int attack = 0;
        int decay = 5;
        int decayMode = 0;
        int filterFrequency = 100;
        int filterResonance = 0;
        int filterAttack = 0;
        int filterDecay = 0;
        int filterEnvelopeAmount = 0;
        int velocityToLevel = 100;
        int velocityToAttack = 0;
        int velocityToStart = 0;
        int velocityToFilterFrequency = 0;
        int sliderParameterNumber = 0;
        int velocityToPitch = 0;
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
