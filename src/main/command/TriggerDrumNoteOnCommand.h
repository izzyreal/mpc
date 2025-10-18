#pragma once
#include "command/Command.h"

namespace mpc::command::context {
    struct TriggerDrumNoteOnContext;
}

namespace mpc::command {

    // Should be renamed to PressProgramPad or similar, because we're not dealing with
    // hardware pad indices from 1 to 16 here, but with the 64 pads of a program.
    class TriggerDrumNoteOnCommand : public Command {
    public:
        TriggerDrumNoteOnCommand(context::TriggerDrumNoteOnContext &ctx, int padIndexWithBank, int velo);
        void execute() override;

    private:
        context::TriggerDrumNoteOnContext &ctx;
        int padIndexWithBank;
        int velo;

        void generateNoteOn(const context::TriggerDrumNoteOnContext &ctx, int note, int velo, int padIndexWithBank);
    };

} // namespace mpc::command
