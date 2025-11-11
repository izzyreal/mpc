#include "PushStopCommand.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushStopCommand::PushStopCommand(Mpc &mpc) : mpc(mpc) {}

void PushStopCommand::execute()
{
    const auto vmpcDirectToDiskRecorderScreen =
        mpc.screens->get<ScreenId::VmpcDirectToDiskRecorderScreen>();
    const auto engineHost = mpc.getEngineHost();

    mpc.clientEventController->clientHardwareEventController
        ->unlockNoteRepeat();

    if (engineHost->isBouncing() &&
        (vmpcDirectToDiskRecorderScreen->getRecord() != 4 ||
         mpc.getHardware()
             ->getButton(hardware::ComponentId::SHIFT)
             ->isPressed()))
    {
        engineHost->stopBouncingEarly();
    }

    mpc.getSequencer()->getTransport()->stop();

    if (!screengroups::isPlayScreen(
            mpc.getLayeredScreen()->getCurrentScreen()))
    {
        mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
    }

    const auto sequencerScreen = mpc.screens->get<ScreenId::SequencerScreen>();
    sequencerScreen->hideFooterLabelAndShowFunctionKeys();

    mpc.getHardware()
        ->getLed(hardware::ComponentId::OVERDUB_LED)
        ->setEnabled(mpc.getSequencer()->getTransport()->isOverdubbing());
    mpc.getHardware()
        ->getLed(hardware::ComponentId::REC_LED)
        ->setEnabled(mpc.getSequencer()->getTransport()->isRecording());
    mpc.getHardware()
        ->getLed(hardware::ComponentId::PLAY_LED)
        ->setEnabled(mpc.getSequencer()->getTransport()->isPlaying());
}
