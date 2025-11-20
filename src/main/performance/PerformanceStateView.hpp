#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Sequence.hpp"
#include "performance/Drum.hpp"

#include <memory>

namespace mpc::performance
{
    struct PerformanceState;

    class PerformanceStateView
    {
    public:
        explicit PerformanceStateView(
            const std::shared_ptr<const PerformanceState> &s) noexcept;

        VelocityOrPressure
            getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex) const;

        bool isProgramPadPressed(ProgramPadIndex) const;
        bool isProgramPadPressed(ProgramPadIndex, ProgramIndex) const;

        std::optional<ProgramPadPressEvent> getMostRecentProgramPadPress(
            ProgramPadIndex,
            const std::vector<PerformanceEventSource> &sourcesToExclude) const;

        int getTotalNoteOnCount() const;

        Program getProg(ProgramIndex) const;
        Program getDrumProgram(DrumBusIndex) const;
        Drum getDrum(DrumBusIndex) const;

    private:
        std::shared_ptr<const PerformanceState> state;
    };
} // namespace mpc::performance
