#include "DeleteAllProgramsScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllProgramsScreen::DeleteAllProgramsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-all-programs", layerIndex)
{
}

void DeleteAllProgramsScreen::function(int j)
{
	init();

	switch (j)
	{
	case 3:
		openScreen("delete-program");
		break;
	case 4:
	{
		sampler->deleteAllPrograms(/*createDefaultProgram=*/true);
        openScreen("delete-program");
		break;
	}
	}
}
