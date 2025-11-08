#include "PopupScreen.hpp"

#include "lcdgui/Label.hpp"
#include "lcdgui/LayeredScreen.hpp"

using namespace mpc::lcdgui::screens::dialog2;

PopupScreen::PopupScreen(Mpc &mpc, int layer)
    : ScreenComponent(mpc, "popup", layer)
{
    addChild(std::make_shared<Label>(mpc, "popup", "", 43, 23, 0));
    findChild<Label>("popup")->setInverted(true);
}

void PopupScreen::show(const PopupConfig &configToUse)
{
    config = configToUse;
    startTime = std::chrono::steady_clock::now();
    setText(config.text);
}

void PopupScreen::setText(std::string text)
{
    if (text.size() > 28)
    {
        text = text.substr(0, 28);
    }

    findChild<Label>("popup")->setText(text);
    config.text = text;
    SetDirty();
}

void PopupScreen::close()
{
    config = PopupConfig();
}

bool PopupScreen::isCloseUponButtonOrPadPressOrDataWheelTurnEnabled() const
{
    return config.closeUponButtonOrPadPressOrDataWheelTurn;
}

void PopupScreen::onTimerCallback()
{
    if (!config.autoCloseDelayMs)
    {
        return;
    }

    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now() - startTime)
                             .count();

    if (elapsed < *config.autoCloseDelayMs)
    {
        return;
    }

    switch (config.behavior.action)
    {
        case PopupBehavior::OnTimeoutAction::Close:
            ls->closeCurrentScreen();
            break;

        case PopupBehavior::OnTimeoutAction::OpenScreen:
            if (config.behavior.targetScreen)
            {
                openScreenById(*config.behavior.targetScreen);
            }
            break;

        case PopupBehavior::OnTimeoutAction::ReturnToLayer:
            if (config.behavior.targetLayer)
            {
                ls->closeRecentScreensUntilReachingLayer(
                    *config.behavior.targetLayer);
            }
            break;

        default:
            break;
    }

    config.autoCloseDelayMs.reset();
}
