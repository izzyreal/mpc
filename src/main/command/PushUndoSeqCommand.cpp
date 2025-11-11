#include "PushUndoSeqCommand.hpp"
#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command
{

    PushUndoSeqCommand::PushUndoSeqCommand(Mpc &mpc) : mpc(mpc) {}

    void PushUndoSeqCommand::execute()
    {
        mpc.getSequencer()->undoSeq();
    }

} // namespace mpc::command
