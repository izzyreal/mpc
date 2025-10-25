#include "Mpc2000XlAllFileScreen.hpp"

#include "disk/AbstractDisk.hpp"

#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/LoadASequenceFromAllScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

Mpc2000XlAllFileScreen::Mpc2000XlAllFileScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "mpc2000xl-all-file", layerIndex)
{
}

void Mpc2000XlAllFileScreen::function(int i)
{

    auto loadScreen = mpc.screens->get<LoadScreen>();

    switch (i)
    {
        case 2:
        {
            auto result = mpc.getDisk()->readSequencesFromAll2(loadScreen->getSelectedFile());

            if (result.has_value())
            {
                auto loadASequenceFromAllScreen = mpc.screens->get<LoadASequenceFromAllScreen>();
                loadASequenceFromAllScreen->sequencesFromAllFile = result.value();
                mpc.getLayeredScreen()->openScreen<LoadASequenceFromAllScreen>();
            }

            break;
        }
        case 3:
            mpc.getLayeredScreen()->openScreen<LoadScreen>();
            break;
        case 4:
        {
            auto on_success = [&]()
            {
                mpc.getLayeredScreen()->openScreen<LoadScreen>();
            };
            mpc.getDisk()->readAll2(loadScreen->getSelectedFile(), on_success);
            break;
        }
    }
}
