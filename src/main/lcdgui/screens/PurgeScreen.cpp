#include "PurgeScreen.hpp"

#include "Mpc.hpp"
#include "SelectDrumScreen.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens;

PurgeScreen::PurgeScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "purge", layerIndex)
{
}

void PurgeScreen::open()
{
    const auto unusedSampleCount = sampler->getUnusedSampleCount();
    findLabel("value")->setTextPadded(unusedSampleCount, " ");
    ls->setFunctionKeysArrangement(unusedSampleCount > 0 ? 1 : 0);
}

void PurgeScreen::function(int f)
{

    switch (f)
    {
        case 0:
            openScreenById(ScreenId::PgmAssignScreen);
            break;
        case 1:
            openScreenById(ScreenId::PgmParamsScreen);
            break;
        case 2:
            openScreenById(ScreenId::DrumScreen);
            break;
        case 3:
        {
            auto selectDrumScreen =
                mpc.screens->get<ScreenId::SelectDrumScreen>();
            selectDrumScreen->redirectScreen = "purge";
            openScreenById(ScreenId::SelectDrumScreen);
            break;
        }
        case 5:
            if (sampler->getUnusedSampleCount() == 0)
            {
                return;
            }
            sampler->purge();
            open();
            break;
    }
}
