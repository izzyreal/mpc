#include "NextSeqCommand.h"
#include "Mpc.hpp"
#include "hardware2/Hardware2.h"
#include "Util.hpp"

namespace mpc::command {

    NextSeqCommand::NextSeqCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void NextSeqCommand::execute() {
        const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();

        if (currentScreenName == "next-seq" || currentScreenName == "next-seq-pad") {
            mpc.getLayeredScreen()->openScreen("sequencer");
            mpc.getHardware2()->getLed("next-seq")->setEnabled(false);
        } else if (currentScreenName == "sequencer" || currentScreenName == "track-mute") {
            Util::initSequence(mpc);
            mpc.getLayeredScreen()->openScreen("next-seq");
            mpc.getHardware2()->getLed("next-seq")->setEnabled(true);
            mpc.getHardware2()->getLed("track-mute")->setEnabled(false);
        }
    }

}
