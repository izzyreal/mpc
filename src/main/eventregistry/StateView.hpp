#pragma once

#include "eventregistry/EventTypes.hpp"

#include <memory>

namespace mpc::eventregistry
{
    struct State;

    class StateView
    {
    public:
        explicit StateView(const std::shared_ptr<const State> s) noexcept
            : state(s)
        {
        }

        bool isProgramPadPressedBySource(ProgramPadIndex, Source) const;
        VelocityOrPressure
            getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex) const;

        bool isProgramPadPressed(ProgramPadIndex) const;
        bool isProgramPadPressed(ProgramPadIndex, ProgramIndex) const;

        std::optional<ProgramPadPressEvent> getMostRecentProgramPadPress(
            ProgramPadIndex, const std::vector<Source> &sourcesToExclude) const;

        int getTotalPressedProgramPadCount() const;

        int getTotalNoteOnCount() const;

    private:
        std::shared_ptr<const State> state;
    };
} // namespace mpc::eventregistry
