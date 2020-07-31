#include "DeleteAllProgramsScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

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
		ls.lock()->openScreen("delete-program");
		break;
	case 4:
	{
		const bool initPgms = true;
		sampler.lock()->deleteAllPrograms(initPgms);
		ls.lock()->openScreen("delete-program");
		break;
	}
	}
}
