#include "PushShiftCommand.h"
#include "Mpc.hpp"
#include "controller/ClientInputControllerBase.h"
#include "hardware2/Hardware2.h"
#include "lcdgui/Field.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushShiftCommand::PushShiftCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushShiftCommand::execute()
    {
        if (mpc.getLayeredScreen()->getCurrentScreenName() == "sequencer" &&
            mpc.getHardware2()->getButton("tap")->isPressed() &&
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
}

