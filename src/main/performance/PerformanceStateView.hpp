#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

#include <memory>

namespace mpc::performance
{
    struct PerformanceState;

    class PerformanceStateView
    {
    public:
        explicit PerformanceStateView(
            std::shared_ptr<const PerformanceState> s) noexcept;

        VelocityOrPressure
            getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex) const;

        bool isProgramPadPressed(ProgramPadIndex) const;
        bool isProgramPadPressed(ProgramPadIndex, ProgramIndex) const;

        std::optional<ProgramPadPressEvent> getMostRecentProgramPadPress(
            ProgramPadIndex,
            const std::vector<PerformanceEventSource> &sourcesToExclude) const;

        int getTotalNoteOnCount() const;

        performance::Program getProgram(DrumBusIndex) const;
        performance::Drum getDrum(DrumBusIndex) const;

    private:
        std::shared_ptr<const PerformanceState> state;
    };
} // namespace mpc::performance
