#pragma once
#include <sequencer/EventRef.hpp>

namespace mpc::sequencer
{
    class ProgramChangeEvent final : public EventRef
    {
    public:
        void setProgram(int i) const;
        int getProgram() const;

        explicit ProgramChangeEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "program-change";
        }
    };
} // namespace mpc::sequencer
