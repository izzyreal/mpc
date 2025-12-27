#include "FileExistsScreen.hpp"

#include "lcdgui/screens/window/SaveASoundScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui;

FileExistsScreen::FileExistsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "file-exists", layerIndex)
{
}

void FileExistsScreen::function(const int i)
{
    switch (i)
    {
        case 2:
            replaceAction();
            break;
        case 3:
            cancelAction();
            break;
        case 4:
            initializeNameScreen();
            openScreenById(ScreenId::NameScreen);
        default:;
    }
}

void FileExistsScreen::initialize(
    const std::function<void()> &replaceActionToUse,
    const std::function<void()> &initializeNameScreenToUse,
    const std::function<void()> &cancelActionToUse)
{
    replaceAction = replaceActionToUse;
    initializeNameScreen = initializeNameScreenToUse;
    cancelAction = cancelActionToUse;
}
