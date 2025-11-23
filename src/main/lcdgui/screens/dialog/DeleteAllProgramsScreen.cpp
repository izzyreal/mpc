#include "DeleteAllProgramsScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllProgramsScreen::DeleteAllProgramsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-all-programs", layerIndex)
{
}

void DeleteAllProgramsScreen::function(const int j)
{
    switch (j)
    {
        case 3:
            openScreenById(ScreenId::DeleteProgramScreen);
            break;
        case 4:
        {
            sampler.lock()->deleteAllPrograms(/*createDefaultProgram=*/true);
            openScreenById(ScreenId::DeleteProgramScreen);
            break;
        }
    }
}
