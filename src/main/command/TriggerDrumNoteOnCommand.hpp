#pragma once
#include "command/Command.hpp"

namespace mpc::command::context {
    struct TriggerDrumNoteOnContext;
}

namespace mpc::command {
    class TriggerDrumNoteOnCommand : public Command {
    public:
        TriggerDrumNoteOnCommand(context::TriggerDrumNoteOnContext &ctx);
        void execute() override;

    private:
        context::TriggerDrumNoteOnContext &ctx;

        void generateNoteOn(const context::TriggerDrumNoteOnContext &ctx);
    };

} // namespace mpc::command
