#include "FileExistsScreen.hpp"

#include "nvram/MidiControlPersistence.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"

#include "file/all/AllParser.hpp"

#include "lcdgui/screens/window/SaveASoundScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;
using namespace mpc::nvram;

FileExistsScreen::FileExistsScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "file-exists", layerIndex)
{
}

void FileExistsScreen::function(int i)
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
            mpc.getLayeredScreen()->openScreen<NameScreen>();
    }
}

void FileExistsScreen::initialize(
    std::function<void()> replaceActionToUse,
    std::function<void()> initializeNameScreenToUse,
    std::function<void()> cancelActionToUse)
{
    replaceAction = replaceActionToUse;
    initializeNameScreen = initializeNameScreenToUse;
    cancelAction = cancelActionToUse;
}
