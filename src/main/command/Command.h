#pragma once

namespace mpc { class Mpc; }

namespace mpc::command {
    class Command
    {
    public:
        virtual ~Command() = default;
        virtual void execute() = 0;
    };
} // namespace mpc::command
