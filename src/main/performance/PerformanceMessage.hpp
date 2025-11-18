#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

#include <functional>
#include <variant>

namespace mpc::performance
{
    struct UpdateDrumProgram
    {
        DrumBusIndex drumBusIndex{};
        ProgramIndex programIndex{};

        UpdateDrumProgram() = default;

        UpdateDrumProgram(const UpdateDrumProgram &) = delete;
        UpdateDrumProgram &operator=(const UpdateDrumProgram &) = delete;

        UpdateDrumProgram(UpdateDrumProgram &&) noexcept = default;
        UpdateDrumProgram &operator=(UpdateDrumProgram &&) noexcept = default;
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

        UpdateNoteParameters(const UpdateNoteParameters &) = delete;
        UpdateNoteParameters &operator=(const UpdateNoteParameters &) = delete;

        UpdateNoteParameters(UpdateNoteParameters &&) noexcept = default;
        UpdateNoteParameters &
        operator=(UpdateNoteParameters &&) noexcept = default;
    };

    struct UpdateStereoMixer
    {
        DrumBusIndex drumBusIndex{};
        ProgramIndex programIndex{NoProgramIndex};

        DrumNoteNumber drumNoteNumber{};

        Value0To100 StereoMixer::*member = nullptr;

        Value0To100 newValue;

        UpdateStereoMixer() = default;

        UpdateStereoMixer(const UpdateStereoMixer &) = delete;
        UpdateStereoMixer &operator=(const UpdateStereoMixer &) = delete;

        UpdateStereoMixer(UpdateStereoMixer &&) noexcept = default;
        UpdateStereoMixer &operator=(UpdateStereoMixer &&) noexcept = default;
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

        UpdateIndivFxMixer(const UpdateIndivFxMixer &) = delete;
        UpdateIndivFxMixer &operator=(const UpdateIndivFxMixer &) = delete;

        UpdateIndivFxMixer(UpdateIndivFxMixer &&) noexcept = default;
        UpdateIndivFxMixer &operator=(UpdateIndivFxMixer &&) noexcept = default;
    };

    using PerformanceMessagePayload =
        std::variant<std::monostate, PhysicalPadPressEvent,
                     PhysicalPadAftertouchEvent, PhysicalPadReleaseEvent,
                     ProgramPadPressEvent, ProgramPadAftertouchEvent,
                     ProgramPadReleaseEvent, NoteOnEvent, NoteAftertouchEvent,
                     NoteOffEvent, UpdateDrumProgram, UpdateNoteParameters,
                     UpdateIndivFxMixer, UpdateStereoMixer>;

    struct PerformanceMessage
    {
        PerformanceMessagePayload payload{};
        PerformanceEventSource source{};
        std::function<void(void *)> action = [](void *) {};

        PerformanceMessage() = default;

        PerformanceMessage(const PerformanceMessage &) = delete;
        PerformanceMessage &operator=(const PerformanceMessage &) = delete;

        PerformanceMessage(PerformanceMessage &&) noexcept = default;
        PerformanceMessage &operator=(PerformanceMessage &&) noexcept = default;
    };
} // namespace mpc::performance
