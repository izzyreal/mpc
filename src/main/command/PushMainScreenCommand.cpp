#include "PushMainScreenCommand.hpp"
#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "hardware/Hardware.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushMainScreenCommand::PushMainScreenCommand(Mpc &mpc) : mpc(mpc) {}

void PushMainScreenCommand::execute()
{

    if (const auto engineHost = mpc.getEngineHost();
        engineHost->isRecordingSound())
    {
        engineHost->stopSoundRecorder();
    }

    mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
    mpc.getSequencer()->setSoloEnabled(mpc.getSequencer()->isSoloEnabled());

    const auto hw = mpc.getHardware();
    hw->getLed(hardware::ComponentId::NEXT_SEQ_LED)->setEnabled(false);
    hw->getLed(hardware::ComponentId::TRACK_MUTE_LED)->setEnabled(false);
}
