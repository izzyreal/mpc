#include "PushFunctionCommand.h"
#include "Mpc.hpp"
#include "lcdgui/screens/window/DirectoryScreen.hpp"
#include "lcdgui/screens/window/EditSoundScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"

namespace mpc::command {

    PushFunctionCommand::PushFunctionCommand(mpc::Mpc &mpc, int i) : mpc(mpc), i(i) {}

    void PushFunctionCommand::execute() {
        // We only handle F4 presses here
        if (i != 3) return;

        auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();

        if (mpc.getLayeredScreen()->getFocusedLayerIndex() == 1)
        {
            if (currentScreenName == "sequence" || currentScreenName == "midi-input" || currentScreenName == "midi-output") {
                mpc.getLayeredScreen()->setPreviousScreenName("sequencer");
            } else if (currentScreenName == "edit-sound") {
                const auto editSoundScreen = mpc.screens->get<lcdgui::screens::window::EditSoundScreen>();
                mpc.getLayeredScreen()->setPreviousScreenName(editSoundScreen->getReturnToScreenName());
            } else if (currentScreenName == "sound") {
                mpc.getLayeredScreen()->setPreviousScreenName(mpc.getSampler()->getPreviousScreenName());
            } else if (currentScreenName == "name") {
                auto nameScreen = mpc.screens->get<lcdgui::screens::window::NameScreen>();
                nameScreen->setEditing(false);
                mpc.getLayeredScreen()->setLastFocus("name", "0");
            } else if (currentScreenName == "directory") {
                const auto directoryScreen = mpc.screens->get<lcdgui::screens::window::DirectoryScreen>();
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
