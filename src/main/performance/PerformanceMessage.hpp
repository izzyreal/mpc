#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

#include "MpcMacros.hpp"

#include <functional>
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
        MPC_NON_COPYABLE(DeleteSoundAndReindex)
    };

    struct AddProgramSound
    {
        ProgramIndex programIndex;
        DrumNoteNumber drumNoteNumber;
        std::vector<std::pair<int, std::string>> localTable;
        std::vector<std::pair<int, std::string>> convertedTable;

        MPC_NON_COPYABLE(AddProgramSound)
    };

    struct UpdateDrumProgram
    {
        DrumBusIndex drumBusIndex{};
        ProgramIndex programIndex{};

        UpdateDrumProgram() = default;

        MPC_NON_COPYABLE(UpdateDrumProgram)
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

        MPC_NON_COPYABLE(UpdateNoteParameters)
    };

    struct UpdateNoteParametersBulk
    {
        ProgramIndex programIndex{};
        DrumNoteNumber drumNoteNumber{};
        NoteParameters noteParameters{};

        UpdateNoteParametersBulk() = default;

        MPC_NON_COPYABLE(UpdateNoteParametersBulk)
    };

    struct UpdateProgramBulk
    {
        ProgramIndex programIndex{};
        Program program{};

        UpdateProgramBulk() = default;

        MPC_NON_COPYABLE(UpdateProgramBulk)
    };

    struct RepairProgramReferences
    {
    };

    struct UpdateProgramMidiProgramChange
    {
        ProgramIndex programIndex;
        int midiProgramChange;

        MPC_NON_COPYABLE(UpdateProgramMidiProgramChange)
    };

    struct UpdateStereoMixer
    {
        DrumBusIndex drumBusIndex{};
        ProgramIndex programIndex{NoProgramIndex};

        DrumNoteNumber drumNoteNumber{};

        Value0To100 StereoMixer::*member = nullptr;

        Value0To100 newValue;

        UpdateStereoMixer() = default;

        MPC_NON_COPYABLE(UpdateStereoMixer)
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

        MPC_NON_COPYABLE(UpdateIndivFxMixer)
    };

    struct SetProgramUsed
    {
        ProgramIndex programIndex;
    };

    using PerformanceMessagePayload = std::variant<
        std::monostate, PhysicalPadPressEvent, PhysicalPadAftertouchEvent,
        PhysicalPadReleaseEvent, ProgramPadPressEvent,
        ProgramPadAftertouchEvent, ProgramPadReleaseEvent, NoteOnEvent,
        NoteAftertouchEvent, NoteOffEvent, UpdateDrumProgram,
        UpdateNoteParameters, UpdateIndivFxMixer, UpdateStereoMixer,
        DeleteSoundAndReindex, AddProgramSound, UpdateNoteParametersBulk,
        UpdateProgramBulk, UpdateProgramMidiProgramChange,
        SetProgramUsed, RepairProgramReferences>;

    struct PerformanceMessage
    {
        PerformanceMessagePayload payload{};
        PerformanceEventSource source{};
        std::function<void(const void *)> action = [](const void *) {};

        PerformanceMessage() = default;
        explicit PerformanceMessage(PerformanceMessagePayload &&payloadToUse)
            : payload{std::move(payloadToUse)}
        {
        }

        MPC_NON_COPYABLE(PerformanceMessage)
    };
} // namespace mpc::performance
