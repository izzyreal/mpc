#pragma once

#include "Command.hpp"
#include "IntTypes.hpp"

namespace mpc::command
{
    class AuditionProgramNoteCommand final : public Command
    {
    public:
        AuditionProgramNoteCommand(Mpc &, DrumBusIndex, DrumNoteNumber,
                                   Velocity);

        void execute() override;

    private:
        Mpc &mpc;
        DrumBusIndex drumBusIndex;
        DrumNoteNumber note;
        Velocity velocity;
    };
} // namespace mpc::command
