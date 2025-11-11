#include "Mpc2000XlAllFileScreen.hpp"

#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"

#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/LoadASequenceFromAllScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

Mpc2000XlAllFileScreen::Mpc2000XlAllFileScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "mpc2000xl-all-file", layerIndex)
{
}

void Mpc2000XlAllFileScreen::function(const int i)
{
    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();

    switch (i)
    {
        case 2:
        {
            auto result = mpc.getDisk()->readSequencesFromAll2(
                loadScreen->getSelectedFile());

            if (result.has_value())
            {
                const auto loadASequenceFromAllScreen =
                    mpc.screens->get<ScreenId::LoadASequenceFromAllScreen>();
                loadASequenceFromAllScreen->sequencesFromAllFile =
                    result.value();
                openScreenById(ScreenId::LoadASequenceFromAllScreen);
            }

            break;
        }
        case 3:
            openScreenById(ScreenId::LoadScreen);
            break;
        case 4:
        {
            auto on_success = [&]
            {
                openScreenById(ScreenId::LoadScreen);
            };
            mpc.getDisk()->readAll2(loadScreen->getSelectedFile(), on_success);
            break;
        }
    }
}
