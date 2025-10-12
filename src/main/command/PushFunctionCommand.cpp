#include "PushFunctionCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "lcdgui/screens/window/DirectoryScreen.hpp"
#include "lcdgui/screens/window/EditSoundScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"

namespace mpc::command {

    PushFunctionCommand::PushFunctionCommand(mpc::Mpc &mpc, int i) : mpc(mpc), i(i) {}

    void PushFunctionCommand::execute() {
        if (i != 3) return;

        auto controls = mpc.getControls();
        controls->setF4Pressed(true);

        auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();

        if (mpc.getLayeredScreen()->getFocusedLayerIndex() == 1)
        {
            if (currentScreenName == "sequence" || currentScreenName == "midi-input" || currentScreenName == "midi-output") {
                mpc.getLayeredScreen()->setPreviousScreenName("sequencer");
            } else if (currentScreenName == "edit-sound") {
                const auto editSoundScreen = mpc.screens->get<lcdgui::screens::window::EditSoundScreen>("edit-sound");
                mpc.getLayeredScreen()->setPreviousScreenName(editSoundScreen->getReturnToScreenName());
            } else if (currentScreenName == "sound") {
                mpc.getLayeredScreen()->setPreviousScreenName(mpc.getSampler()->getPreviousScreenName());
            } else if (currentScreenName == "program") {
                mpc.getLayeredScreen()->setPreviousScreenName(mpc.getPreviousSamplerScreenName());
                mpc.setPreviousSamplerScreenName("");
            } else if (currentScreenName == "name") {
                auto nameScreen = mpc.screens->get<lcdgui::screens::window::NameScreen>("name");
                nameScreen->setEditing(false);
                mpc.getLayeredScreen()->setLastFocus("name", "0");
            } else if (currentScreenName == "directory") {
                const auto directoryScreen = mpc.screens->get<lcdgui::screens::window::DirectoryScreen>("directory");
                mpc.getLayeredScreen()->setPreviousScreenName(directoryScreen->getPreviousScreenName());
            }
        }

        if (mpc.getLayeredScreen()->getFocusedLayerIndex() == 1 ||
            mpc.getLayeredScreen()->getFocusedLayerIndex() == 2 ||
            mpc.getLayeredScreen()->getFocusedLayerIndex() == 3) {
            mpc.getLayeredScreen()->openScreen(mpc.getLayeredScreen()->getPreviousScreenName());
        }
    }

} // namespace mpc::command
