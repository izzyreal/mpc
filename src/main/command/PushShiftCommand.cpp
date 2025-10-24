#include "PushShiftCommand.hpp"
#include "Mpc.hpp"
#include "controller/ClientHardwareControllerBase.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/Field.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command
{

    PushShiftCommand::PushShiftCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushShiftCommand::execute()
    {
        if (mpc.getLayeredScreen()->getCurrentScreenName() == "sequencer" &&
            mpc.getHardware()->getButton(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT)->isPressed() &&
            mpc.getSequencer()->isPlaying())
        {
            mpc.inputController->lockNoteRepeat();
        }

        auto field = mpc.getLayeredScreen()->getFocusedField();

        if (!field || !field->isTypeModeEnabled())
        {
            return;
        }

        field->disableTypeMode();

        const auto split = field->getActiveSplit();

        if (split != -1)
        {
            field->setSplit(true);
            field->setActiveSplit(split);
        }
    }
} // namespace mpc::command
