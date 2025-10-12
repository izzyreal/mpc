#pragma once
#include "command/Command.h"

namespace mpc::controls {
struct PushPadContext;
}

namespace mpc::command {

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
