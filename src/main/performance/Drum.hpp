#pragma once

#include "IntTypes.hpp"

#include <array>

namespace mpc::performance
{
    struct StereoMixer {
        DrumMixerLevel level{MaxDrumMixerLevel};
        DrumMixerPanning panning{PanningCenter};
    };

    struct IndivFxMixer {
        DrumMixerLevel individualOutLevel{MaxDrumMixerLevel};
        DrumMixerLevel fxSendLevel{MinDrumMixerLevel};
        DrumMixerIndividualOutput individualOutput{MinDrumMixerIndividualOut};
        DrumMixerIndividualFxPath fxPath{MinDrumMixerFxPath};
        bool followStereo = false;
    };

    struct NoteParameters {
        StereoMixer stereoMixer{};
        IndivFxMixer indivFxMixer{};
        int soundIndex = -1;
        int soundGenerationMode = 0;
        int velocityRangeLower = 0;
        DrumNoteNumber optionalNoteA{NoDrumNoteAssigned};
        int velocityRangeUpper = 0;
        DrumNoteNumber optionalNoteB{NoDrumNoteAssigned};
        int8_t voiceOverlapMode = 0;
        DrumNoteNumber muteAssignA{NoDrumNoteAssigned};
        DrumNoteNumber muteAssignB{NoDrumNoteAssigned};
        int tune = 0;
        int attack = 0;
        int decay = 0;
        int decayMode = 0;
        int filterFrequency = 0;
        int filterResonance = 0;
        int filterAttack = 0;
        int filterDecay = 0;
        int filterEnvelopeAmount = 0;
        int velocityToLevel = 0;
        int velocityToAttack = 0;
        int velocityToStart = 0;
        int velocityToFilterFrequency = 0;
        int sliderParameterNumber = 0;
        int velocityToPitch = 0;
        int index = 0;
    };

    struct Pad {
        DrumNoteNumber note{NoDrumNoteAssigned};
        ProgramPadIndex index{0};
    };

    struct Slider {
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

    struct Program {
        std::array<NoteParameters, 64> noteParameters;
        std::array<Pad, 64> pads;
        Slider slider{};
        int midiProgramChange = 0;
    };

    struct Drum {
        Program program{};
        std::array<StereoMixer, 64> stereoMixers;
        std::array<IndivFxMixer, 64> indivFxMixers;
        std::array<std::pair<DrumNoteNumber, DrumNoteNumber>, 64> simultaneousNotesA{
            std::pair{NoDrumNoteAssigned, NoDrumNoteAssigned}
        };
        std::array<std::pair<DrumNoteNumber, DrumNoteNumber>, 64> simultaneousNotesB{
            std::pair{NoDrumNoteAssigned, NoDrumNoteAssigned}
        };
        bool receivePgmChange = false;
        bool receiveMidiVolume = false;
        MidiValue lastReceivedMidiVolume{MaxMidiValue};
    };
}

