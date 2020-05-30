#include "DeleteProgramScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;
using namespace std;

DeleteProgramScreen::DeleteProgramScreen(const int layerIndex)
	: ScreenComponent("delete-program", layerIndex)
{
}

void DeleteProgramScreen::function(int i)
{
    init();
	
	switch (i)
    {
    case 2:
        ls.lock()->openScreen("delete-all-programs");
        break;
    case 3:
        ls.lock()->openScreen("program");
        break;
    case 4:
        if (sampler.lock()->getProgramCount() > 1)
        {
			sampler.lock()->deleteProgram(dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(deletePgm).lock()));
        } else {
			const bool initPgms = true;
			sampler.lock()->deleteAllPrograms(initPgms);
		}
        ls.lock()->openScreen("program");
        break;
    }
}

void DeleteProgramScreen::turnWheel(int i)
{
    init();
    if (param.compare("pgm") == 0)
    {
		setDeletePgm(deletePgm + i);
	}
}

void DeleteProgramScreen::displayDeletePgm()
{
    findField("pgm").lock()->setText(StrUtil::padLeft(to_string(deletePgm + 1), " ", 2) + "-" + dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(deletePgm).lock())->getName());
}

void DeleteProgramScreen::setDeletePgm(int i)
{
    if (i < 0 || i >= sampler.lock()->getProgramCount())
    {
        return;
    }

    deletePgm = i;
    displayDeletePgm();
}
