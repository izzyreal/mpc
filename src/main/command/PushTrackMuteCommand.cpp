#include "PushTrackMuteCommand.h"
#include "Mpc.hpp"
#include "Util.hpp"
#include "hardware/Hardware.h"

namespace mpc::command {

    PushTrackMuteCommand::PushTrackMuteCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushTrackMuteCommand::execute() {
        const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();

        if (currentScreenName == "track-mute") {
            auto previous = mpc.getLayeredScreen()->getPreviousScreenName();
            if (previous == "next-seq" || previous == "next-seq-pad") {
                mpc.getLayeredScreen()->openScreen<NextSeqScreen>();
            } else {
                mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            }
            mpc.getHardware()->getLed(hardware::ComponentId::TRACK_MUTE_LED)->setEnabled(false);
        } else if (currentScreenName == "next-seq" || currentScreenName == "next-seq-pad" || currentScreenName == "sequencer") {
            Util::initSequence(mpc);
            mpc.getLayeredScreen()->openScreen<TrMuteScreen>();
            mpc.getHardware()->getLed(hardware::ComponentId::TRACK_MUTE_LED)->setEnabled(true);
        }
    }

}
