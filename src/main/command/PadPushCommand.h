#pragma once
#include "command/Command.h"

namespace mpc::controls {
struct PadPushContext;
}

namespace mpc::command {

    class PadPushCommand : public Command {
    public:
        PadPushCommand(mpc::controls::PadPushContext &ctx, int padIndexWithBank, int velo);
        void execute() override;

    private:
        mpc::controls::PadPushContext &ctx;
        int padIndexWithBank;
        int velo;

        void generateNoteOn(const mpc::controls::PadPushContext &ctx, int note, int velo, int padIndexWithBank);
    };

} // namespace mpc::command
