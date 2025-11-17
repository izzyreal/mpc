#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

#include <functional>
#include <variant>

namespace mpc::performance
{
    //
    // Move-only payload structs
    //
    struct UpdateDrumProgram
    {
        DrumBusIndex drumBusIndex{};
        ProgramIndex programIndex{};
        Program performanceProgram{};

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
        NoteParameters performanceNoteParameters{};

        UpdateNoteParameters() = default;

        UpdateNoteParameters(const UpdateNoteParameters &) = delete;
        UpdateNoteParameters &operator=(const UpdateNoteParameters &) = delete;

        UpdateNoteParameters(UpdateNoteParameters &&) noexcept = default;
        UpdateNoteParameters &operator=(UpdateNoteParameters &&) noexcept = default;
    };

    struct UpdateStereoMixer
    {
        StereoMixer stereoMixer{};

        UpdateStereoMixer() = default;

        UpdateStereoMixer(const UpdateStereoMixer &) = delete;
        UpdateStereoMixer &operator=(const UpdateStereoMixer &) = delete;

        UpdateStereoMixer(UpdateStereoMixer &&) noexcept = default;
        UpdateStereoMixer &operator=(UpdateStereoMixer &&) noexcept = default;
    };

    struct UpdateIndivFxMixer
    {
        IndivFxMixer indivFxMixer{};

        UpdateIndivFxMixer() = default;

        UpdateIndivFxMixer(const UpdateIndivFxMixer &) = delete;
        UpdateIndivFxMixer &operator=(const UpdateIndivFxMixer &) = delete;

        UpdateIndivFxMixer(UpdateIndivFxMixer &&) noexcept = default;
        UpdateIndivFxMixer &operator=(UpdateIndivFxMixer &&) noexcept = default;
    };

    //
    // Variant payload
    //
    using PerformanceMessagePayload =
        std::variant<
            std::monostate,
            PhysicalPadPressEvent,
            PhysicalPadAftertouchEvent,
            PhysicalPadReleaseEvent,
            ProgramPadPressEvent,
            ProgramPadAftertouchEvent,
            ProgramPadReleaseEvent,
            NoteOnEvent,
            NoteAftertouchEvent,
            NoteOffEvent,
            UpdateDrumProgram,
            UpdateNoteParameters,
            UpdateIndivFxMixer,
            UpdateStereoMixer>;

    //
    // Move-only message
    //
    struct PerformanceMessage
    {
        PerformanceMessagePayload payload{};
        PerformanceEventSource source{};
        std::function<void(void*)> action = [](void*) {};

        PerformanceMessage() = default;

        PerformanceMessage(const PerformanceMessage &) = delete;
        PerformanceMessage &operator=(const PerformanceMessage &) = delete;

        PerformanceMessage(PerformanceMessage &&) noexcept = default;
        PerformanceMessage &operator=(PerformanceMessage &&) noexcept = default;
    };

} // namespace mpc::performance
