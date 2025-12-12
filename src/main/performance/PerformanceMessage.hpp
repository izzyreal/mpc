#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

#include <variant>

namespace mpc::sampler
{
    class NoteParameters;
}

namespace mpc::performance
{
    struct DeleteSoundAndReindex
    {
        int deletedIndex;
    };

    struct AddProgramSound
    {
        ProgramIndex programIndex;
        DrumNoteNumber drumNoteNumber;
        std::vector<std::pair<int, std::string>> localTable;
        std::vector<std::pair<int, std::string>> convertedTable;
    };

    struct UpdateDrumProgram
    {
        DrumBusIndex drumBusIndex{};
        ProgramIndex programIndex{};

        UpdateDrumProgram() = default;
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

        UpdateNoteParameters() = default;
    };

    struct UpdateNoteParametersBySnapshot
    {
        ProgramIndex programIndex{};
        DrumNoteNumber drumNoteNumber{};

        NoteParameters snapshot;

        UpdateNoteParametersBySnapshot() = default;
    };

    struct UpdateNoteParametersBulk
    {
        ProgramIndex programIndex{};
        DrumNoteNumber drumNoteNumber{};
        NoteParameters noteParameters{};

        UpdateNoteParametersBulk() = default;
    };

    struct UpdateProgramBulk
    {
        ProgramIndex programIndex{};
        Program program{};

        UpdateProgramBulk() = default;
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

        UpdateStereoMixer() = default;
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

        UpdateIndivFxMixer() = default;
    };

    struct SetProgramUsed
    {
        ProgramIndex programIndex;
    };

    struct UpdateDrumBulk
    {
        Drum drum{};
    };

    using PerformanceMessagePayload = std::variant<
        std::monostate, PhysicalPadPressEvent, PhysicalPadAftertouchEvent,
        PhysicalPadReleaseEvent, ProgramPadPressEvent,
        ProgramPadAftertouchEvent, ProgramPadReleaseEvent, NoteOnEvent,
        NoteAftertouchEvent, NoteOffEvent, UpdateDrumProgram,
        UpdateNoteParameters, UpdateIndivFxMixer, UpdateStereoMixer,
        DeleteSoundAndReindex, AddProgramSound, UpdateNoteParametersBulk,
        UpdateProgramBulk, UpdateProgramMidiProgramChange, SetProgramUsed,
        RepairProgramReferences, UpdateDrumBulk,
        UpdateNoteParametersBySnapshot>;

    struct PerformanceMessage
    {
        PerformanceMessagePayload payload{};
        PerformanceEventSource source{};

        PerformanceMessage() = default;
        explicit PerformanceMessage(PerformanceMessagePayload &&payloadToUse)
            : payload{std::move(payloadToUse)}
        {
        }
    };
} // namespace mpc::performance
