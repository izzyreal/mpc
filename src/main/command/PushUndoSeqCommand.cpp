#include "PushUndoSeqCommand.hpp"
#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushUndoSeqCommand::PushUndoSeqCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushUndoSeqCommand::execute() {
        mpc.getSequencer()->undoSeq();
    }

}
