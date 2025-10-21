#pragma once
#include "command/Command.hpp"

namespace mpc::command::context {
    struct NoteInputScreenUpdateContext;
}

namespace mpc::command {

    class NoteInputScreenUpdateCommand : public Command {
    public:
        NoteInputScreenUpdateCommand(context::NoteInputScreenUpdateContext &ctx, const int note);
        void execute() override;

    private:
        context::NoteInputScreenUpdateContext &ctx;
        const int note;
    };

} // namespace mpc::command
