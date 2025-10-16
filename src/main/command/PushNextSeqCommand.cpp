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
            mpc.getHardware()->getLed(hardware::ComponentId::NEXT_SEQ_LED)->setEnabled(false);
        } else if (currentScreenName == "sequencer" || currentScreenName == "track-mute") {
            Util::initSequence(mpc);
            mpc.getLayeredScreen()->openScreen("next-seq");
            mpc.getHardware()->getLed(hardware::ComponentId::NEXT_SEQ_LED)->setEnabled(true);
            mpc.getHardware()->getLed(hardware::ComponentId::TRACK_MUTE_LED)->setEnabled(false);
        }
    }

}
