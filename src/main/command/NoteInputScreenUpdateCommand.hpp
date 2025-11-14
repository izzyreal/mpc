#pragma once
#include "IntTypes.hpp"
#include "command/Command.hpp"

namespace mpc::command::context
{
    struct NoteInputScreenUpdateContext;
}

namespace mpc::command
{
    class NoteInputScreenUpdateCommand final : public Command
    {
    public:
        NoteInputScreenUpdateCommand(
            context::NoteInputScreenUpdateContext &ctxToUse,
            NoteNumber noteToUse);
        void execute() override;

    private:
        context::NoteInputScreenUpdateContext &ctx;
        const DrumNoteNumber note;
    };
} // namespace mpc::command
