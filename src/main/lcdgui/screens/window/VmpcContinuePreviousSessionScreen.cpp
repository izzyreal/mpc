#include "VmpcContinuePreviousSessionScreen.hpp"

#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"
#include "Mpc.hpp"
#include "lcdgui/Label.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

VmpcContinuePreviousSessionScreen::VmpcContinuePreviousSessionScreen(
    Mpc &m, const int layer)
    : ScreenComponent(m, "vmpc-continue-previous-session", layer)
{
    const auto label0 = std::make_shared<Label>(
        mpc, "line0", "An auto-saved previous session", 24, 10, 32 * 6);
    addChild(label0);
    const auto label1 = std::make_shared<Label>(
        mpc, "line1", "was found. Do you want to", 24, 19, 32 * 6);
    addChild(label1);
    const auto label2 = std::make_shared<Label>(
        mpc, "line2", "continue the session? Also see", 24, 28, 32 * 6);
    addChild(label2);
    const auto label3 = std::make_shared<Label>(
        mpc, "line3", "AUTSAV screen (Shift + 0, F3).", 24, 37, 32 * 6);
    addChild(label3);
}

void VmpcContinuePreviousSessionScreen::function(const int i)
{
    const auto autoSaveScreen =
        mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();

    switch (i)
    {
        case 1:
            // NO
            mpc.getLayeredScreen()->closeRecentScreensUntilReachingLayer(0);
            break;
        case 2:
            // YES
            mpc.getLayeredScreen()->closeRecentScreensUntilReachingLayer(0);
            std::thread(
                [this]()
                {
                    restoreAutoSavedStateAction();
                })
                .detach();
            break;
        case 3:
            // NEVER
            autoSaveScreen->setAutoLoadOnStart(0);
            mpc.getLayeredScreen()->closeRecentScreensUntilReachingLayer(0);
            break;
        case 4:
            // ALWAYS
            autoSaveScreen->setAutoLoadOnStart(2);
            mpc.getLayeredScreen()->closeRecentScreensUntilReachingLayer(0);
            std::thread(
                [this]()
                {
                    restoreAutoSavedStateAction();
                })
                .detach();
            break;
        default:
            break;
    }
}

void VmpcContinuePreviousSessionScreen::setRestoreAutoSavedStateAction(
    const std::function<void()> &action)
{
    restoreAutoSavedStateAction = action;
}
