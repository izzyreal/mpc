#include "VmpcContinuePreviousSessionScreen.hpp"

#include "../VmpcAutoSaveScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

VmpcContinuePreviousSessionScreen::VmpcContinuePreviousSessionScreen(mpc::Mpc &m, int layer)
: ScreenComponent(m, "vmpc-continue-previous-session", layer)
{
    auto label0 = std::make_shared<Label>(mpc, "line0", "An auto-saved previous session", 24, 10, 32 * 6);
    addChild(label0);
    auto label1 = std::make_shared<Label>(mpc, "line1", "was found. Do you want to", 24, 19, 32 * 6);
    addChild(label1);
    auto label2 = std::make_shared<Label>(mpc, "line2", "continue the session? Also see", 24, 28, 32 * 6);
    addChild(label2);
    auto label3 = std::make_shared<Label>(mpc, "line3", "AUTSAV screen (Shift + 0, F3).", 24, 37, 32 * 6);
    addChild(label3);
}

void VmpcContinuePreviousSessionScreen::function(int i)
{
    auto autoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");
    auto previousScreen = ls->getPreviousScreenName();

    switch (i) {
        case 1:
            // NO
            openScreen(previousScreen);
            break;
        case 2:
            // YES
            restoreAutoSavedStateAction();
            openScreen(previousScreen);
            break;
        case 3:
            // NEVER
            autoSaveScreen->setAutoLoadOnStart(0);
            openScreen(previousScreen);
            break;
        case 4:
            // ALWAYS
            autoSaveScreen->setAutoLoadOnStart(2);
            restoreAutoSavedStateAction();
            openScreen(previousScreen);
            break;
        default:
            break;
    }
}

void VmpcContinuePreviousSessionScreen::open()
{
}

void VmpcContinuePreviousSessionScreen::setRestoreAutoSavedStateAction(std::function<void()> action)
{
    restoreAutoSavedStateAction = action;
}
