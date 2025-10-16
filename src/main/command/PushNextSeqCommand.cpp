#include "PushNextSeqCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"
#include "Util.hpp"

namespace mpc::command {

    PushNextSeqCommand::PushNextSeqCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushNextSeqCommand::execute() {
        const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();

        if (currentScreenName == "next-seq" || currentScreenName == "next-seq-pad") {
            mpc.getLayeredScreen()->openScreen("sequencer");
            mpc.getHardware()->getLed("next-seq")->setEnabled(false);
        } else if (currentScreenName == "sequencer" || currentScreenName == "track-mute") {
            Util::initSequence(mpc);
            mpc.getLayeredScreen()->openScreen("next-seq");
            mpc.getHardware()->getLed("next-seq")->setEnabled(true);
            mpc.getHardware()->getLed("track-mute")->setEnabled(false);
        }
    }

}
