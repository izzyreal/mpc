#pragma once

#include "IntTypes.hpp"
#include "sampler/VoiceOverlapMode.hpp"

#include <array>

namespace mpc::performance
{
    struct StereoMixer
    {
        StereoMixer()
        {
            resetValuesToDefaults();
        }

        DrumMixerLevel level;
        DrumMixerPanning panning;

        void resetValuesToDefaults()
        {
            level = MaxDrumMixerLevel;
            panning = PanningCenter;
        }
    };

    struct IndivFxMixer
    {
        IndivFxMixer()
        {
            resetValuesToDefaults();
        }
        DrumMixerLevel individualOutLevel;
        DrumMixerLevel fxSendLevel;
        DrumMixerIndividualOutput individualOutput;
        DrumMixerIndividualFxPath fxPath;
        bool followStereo;

        void resetValuesToDefaults()
        {
            individualOutLevel = MaxDrumMixerLevel;
            fxSendLevel = MinDrumMixerLevel;
            individualOutput = MinDrumMixerIndividualOut;
            fxPath = MinDrumMixerFxPath;
            followStereo = false;
        }
    };

    struct NoteParameters
    {
        NoteParameters()
        {
            resetValuesToDefaults();
        }

        DrumNoteNumber noteNumber;
        StereoMixer stereoMixer;
        IndivFxMixer indivFxMixer;
        DrumNoteNumber optionalNoteA;
        DrumNoteNumber optionalNoteB;
        DrumNoteNumber muteAssignB;

        int16_t soundIndex;

        int8_t soundGenerationMode;
        int8_t velocityRangeLower;
        int8_t velocityRangeUpper;
        int8_t tune;
        int8_t attack;
        int8_t decay;
        int8_t decayMode;
        int8_t filterFrequency;
        int8_t filterResonance;
        int8_t filterAttack;
        int8_t filterDecay;
        int8_t filterEnvelopeAmount;
        int8_t velocityToLevel;
        int8_t velocityToAttack;
        int8_t velocityToStart;
        int8_t velocityToFilterFrequency;
        int8_t sliderParameterNumber;
        int8_t velocityToPitch;
        sampler::VoiceOverlapMode voiceOverlapMode;
        DrumNoteNumber muteAssignA;

        void resetValuesToDefaults()
        {
            noteNumber = NoDrumNoteAssigned;
            stereoMixer = StereoMixer();
            indivFxMixer = IndivFxMixer();
            optionalNoteA = NoDrumNoteAssigned;
            optionalNoteB = NoDrumNoteAssigned;
            muteAssignB = NoDrumNoteAssigned;

            soundIndex = -1;

            soundGenerationMode = 0;
            velocityRangeLower = 44;
            velocityRangeUpper = 88;
            tune = 0;
            attack = 0;
            decay = 5;
            decayMode = 0;
            filterFrequency = 100;
            filterResonance = 0;
            filterAttack = 0;
            filterDecay = 0;
            filterEnvelopeAmount = 0;
            velocityToLevel = 100;
            velocityToAttack = 0;
            velocityToStart = 0;
            velocityToFilterFrequency = 0;
            sliderParameterNumber = 0;
            velocityToPitch = 0;
            voiceOverlapMode = sampler::VoiceOverlapMode::POLY;
            muteAssignA = NoDrumNoteAssigned;
        }
    };

    struct Pad
    {
        ProgramPadIndex index;
        DrumNoteNumber note;

        Pad()
        {
            resetValuesToDefaults();
        }

        void resetValuesToDefaults()
        {
            index = NoProgramPadIndex;
            note = NoDrumNoteAssigned;
        }
    };

    struct Slider
    {
        Slider()
        {
            resetValuesToDefaults();
        }

        DrumNoteNumber assignNote;
        int parameter;
        int tuneLowRange;
        int tuneHighRange;
        int decayLowRange;
        int decayHighRange;
        int attackLowRange;
        int attackHighRange;
        int filterLowRange;
        int filterHighRange;
        int controlChange;

        void resetValuesToDefaults()
        {
            assignNote = {NoDrumNoteAssigned};
            parameter = 0;
            tuneLowRange = -120;
            tuneHighRange = 120;
            decayLowRange = 12;
            decayHighRange = 45;
            attackLowRange = 0;
            attackHighRange = 20;
            filterLowRange = -50;
            filterHighRange = 50;
            controlChange = 0;
        }
    };

    struct Program
    {
        std::array<NoteParameters, Mpc2000XlSpecs::PROGRAM_PAD_COUNT>
            noteParameters;
        std::array<Pad, Mpc2000XlSpecs::PROGRAM_PAD_COUNT> pads;

        Slider slider;
        int midiProgramChange;
        bool used;

        Program()
        {
            resetValuesToDefaults();
        }

        NoteParameters getNoteParameters(const DrumNoteNumber noteNumber) const
        {
            return noteParameters[noteNumber.get() - MinDrumNoteNumber.get()];
        }

        void resetValuesToDefaults()
        {
            used = false;

            for (size_t i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; ++i)
            {
                noteParameters[i] = NoteParameters();
                noteParameters[i].noteNumber = DrumNoteNumber(
                    static_cast<uint8_t>(i) + MinDrumNoteNumber.get());
                pads[i] = Pad();
                pads[i].index = ProgramPadIndex(static_cast<int8_t>(i));
            }

            slider = Slider();
            midiProgramChange = 1;
        }
    };

    struct Drum
    {
        DrumBusIndex drumBusIndex;
        ProgramIndex programIndex;
        std::array<StereoMixer, Mpc2000XlSpecs::PROGRAM_PAD_COUNT>
            stereoMixers{};
        std::array<IndivFxMixer, Mpc2000XlSpecs::PROGRAM_PAD_COUNT>
            indivFxMixers{};
        std::array<std::pair<DrumNoteNumber, DrumNoteNumber>,
                   Mpc2000XlSpecs::PROGRAM_PAD_COUNT>
            simultaneousNotesA{
                std::pair{NoDrumNoteAssigned, NoDrumNoteAssigned}};
        std::array<std::pair<DrumNoteNumber, DrumNoteNumber>,
                   Mpc2000XlSpecs::PROGRAM_PAD_COUNT>
            simultaneousNotesB{
                std::pair{NoDrumNoteAssigned, NoDrumNoteAssigned}};
        bool receivePgmChangeEnabled = false;
        bool receiveMidiVolumeEnabled = false;
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

        Drum()
        {
            programIndex = ProgramIndex{0};
        }
    };
} // namespace mpc::performance
