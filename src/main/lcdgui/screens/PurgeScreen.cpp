#include "PurgeScreen.hpp"

#include "SelectDrumScreen.hpp"

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
            mpc.getLayeredScreen()->openScreen<PgmAssignScreen>();
            break;
        case 1:
            mpc.getLayeredScreen()->openScreen<PgmParamsScreen>();
            break;
        case 2:
            mpc.getLayeredScreen()->openScreen<DrumScreen>();
            break;
        case 3:
        {
            auto selectDrumScreen = mpc.screens->get<SelectDrumScreen>();
            selectDrumScreen->redirectScreen = "purge";
            mpc.getLayeredScreen()->openScreen<SelectDrumScreen>();
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
