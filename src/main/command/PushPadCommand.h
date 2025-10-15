#pragma once
#include "command/Command.h"

namespace mpc::controls {
    struct PushPadContext;
}

namespace mpc::command {

    // Should be renamed to PressProgramPad or similar, because we're not dealing with
    // hardware pad indices from 1 to 16 here, but with the 64 pads of a program.
    class PushPadCommand : public Command {
    public:
        PushPadCommand(mpc::controls::PushPadContext &ctx, int padIndexWithBank, int velo);
        void execute() override;

    private:
        mpc::controls::PushPadContext &ctx;
        int padIndexWithBank;
        int velo;

        void generateNoteOn(const mpc::controls::PushPadContext &ctx, int note, int velo, int padIndexWithBank);
    };

} // namespace mpc::command
