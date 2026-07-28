#pragma once

#include "Command.hpp"

namespace mpc::command
{
    class ReleaseProgramNoteAuditionCommand final : public Command
    {
    public:
        explicit ReleaseProgramNoteAuditionCommand(Mpc &);

        void execute() override;

    private:
        Mpc &mpc;
    };
} // namespace mpc::command
