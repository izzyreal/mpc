#include "UndoSeqCommand.h"
#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    UndoSeqCommand::UndoSeqCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void UndoSeqCommand::execute() {
        mpc.getSequencer()->undoSeq();
    }

}
