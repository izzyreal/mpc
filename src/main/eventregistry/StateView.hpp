#pragma once

#include "eventregistry/EventTypes.hpp"

#include <memory>

namespace mpc::eventregistry
{
    struct PerformanceState;

    class StateView
    {
    public:
        explicit StateView(const std::shared_ptr<const PerformanceState> s) noexcept
            : state(s)
        {
        }

        bool isProgramPadPressedBySource(ProgramPadIndex, PerformanceEventSource) const;
        VelocityOrPressure
            getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex) const;

        bool isProgramPadPressed(ProgramPadIndex) const;
        bool isProgramPadPressed(ProgramPadIndex, ProgramIndex) const;

        std::optional<ProgramPadPressEvent> getMostRecentProgramPadPress(
            ProgramPadIndex, const std::vector<PerformanceEventSource> &sourcesToExclude) const;

        int getTotalPressedProgramPadCount() const;

        int getTotalNoteOnCount() const;

    private:
        std::shared_ptr<const PerformanceState> state;
    };
} // namespace mpc::eventregistry
