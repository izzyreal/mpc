#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

#include <variant>
#include <vector>
#include <utility>
#include <string>

namespace mpc::sampler
{
    class NoteParameters;
}

namespace mpc::performance
{
    using SoundTable = std::vector<std::pair<int, std::string>>;

    struct DeleteSoundAndReindex
    {
        int deletedIndex;
    };

    struct AddProgramSound
    {
        ProgramIndex programIndex;
        SoundTable *pgmFileSoundTable;
        SoundTable *samplerSoundTable;
    };

    struct UpdateDrumProgram
    {
        DrumBusIndex drumBusIndex{};
        ProgramIndex programIndex{};

    };

    struct UpdateNoteParameters
    {
        ProgramIndex programIndex{};
        DrumNoteNumber drumNoteNumber{};

        int8_t NoteParameters::*int8_tMemberToUpdate = nullptr;
        int16_t NoteParameters::*int16_tMemberToUpdate = nullptr;
        DrumNoteNumber NoteParameters::*drumNoteMemberToUpdate = nullptr;
        sampler::VoiceOverlapMode NoteParameters::*
            voiceOverlapModeMemberToUpdate = nullptr;

        int8_t int8_tValue;
        int16_t int16_tValue;
        DrumNoteNumber drumNoteValue;
        sampler::VoiceOverlapMode voiceOverlapMode;

    };

    struct UpdateNoteParametersBySnapshot
    {
        ProgramIndex programIndex{};
        DrumNoteNumber drumNoteNumber{};

        NoteParameters snapshot;

    };

    struct UpdateAllNoteParametersBulk
    {
        ProgramIndex programIndex{};
        std::array<NoteParameters, Mpc2000XlSpecs::PROGRAM_PAD_COUNT> snapshot;
    };

    struct UpdateProgramBulk
    {
        ProgramIndex programIndex{};
        Program program{};

    };

    struct RepairProgramReferences
    {
    };

    struct UpdateProgramMidiProgramChange
    {
        ProgramIndex programIndex;
        int midiProgramChange;
    };

    struct UpdateStereoMixer
    {
        DrumBusIndex drumBusIndex{};
        ProgramIndex programIndex{NoProgramIndex};

        DrumNoteNumber drumNoteNumber{};

        Value0To100 StereoMixer::*member = nullptr;

        Value0To100 newValue;

    };

    struct UpdateIndivFxMixer
    {
        DrumBusIndex drumBusIndex{};
        ProgramIndex programIndex{NoProgramIndex};

        DrumNoteNumber drumNoteNumber{};

        Value0To100 IndivFxMixer::*value0To100Member = nullptr;
        DrumMixerIndividualOutput IndivFxMixer::*individualOutputMember =
            nullptr;
        DrumMixerIndividualFxPath IndivFxMixer::*individualFxPathMember =
            nullptr;
        bool IndivFxMixer::*followStereoMember = nullptr;

        Value0To100 newValue;
        DrumMixerIndividualOutput individualOutput;
        DrumMixerIndividualFxPath individualFxPath;
        bool followStereo;

    };

    struct SetProgramUsed
    {
        ProgramIndex programIndex;
    };

    struct UpdateDrumBulk
    {
        Drum drum{};
    };

    struct SetPadNote
    {
        ProgramIndex programIndex{};
        ProgramPadIndex programPadIndex{};
        DrumNoteNumber drumNoteNumber{};
    };

    struct ActivateQuantizedLock
    {
    };

    using PerformanceMessage = std::variant<
        std::monostate, PhysicalPadPressEvent, PhysicalPadAftertouchEvent,
        PhysicalPadReleaseEvent, ProgramPadPressEvent,
        ProgramPadAftertouchEvent, ProgramPadReleaseEvent, NoteOnEvent,
        NoteAftertouchEvent, NoteOffEvent, UpdateDrumProgram,
        UpdateNoteParameters, UpdateIndivFxMixer, UpdateStereoMixer,
        DeleteSoundAndReindex, AddProgramSound, UpdateProgramBulk,
        UpdateProgramMidiProgramChange, SetProgramUsed, RepairProgramReferences,
        UpdateDrumBulk, UpdateNoteParametersBySnapshot, ActivateQuantizedLock,
        UpdateAllNoteParametersBulk, SetPadNote>;
} // namespace mpc::performance
