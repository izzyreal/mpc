#include "DeleteAllSoundScreen.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllSoundScreen::DeleteAllSoundScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-all-sound", layerIndex)
{
}

void DeleteAllSoundScreen::function(const int i)
{
    switch (i)
    {
        case int(3):
            openScreenById(ScreenId::DeleteSoundScreen);
            break;
        case 4:
            sampler.lock()->deleteAllSamples();
            ls.lock()->openScreen(sampler.lock()->getPreviousScreenName());
            break;
    }
}
