#include "DeleteProgramScreen.hpp"

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;
using namespace moduru::lang;
using namespace std;

DeleteProgramScreen::DeleteProgramScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-program", layerIndex)
{
}

void DeleteProgramScreen::open()
{
    auto allPrograms = sampler.lock()->getPrograms();

    init();

    pgm = mpcSoundPlayerChannel->getProgram();

    displayPgm();
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
			sampler.lock()->deleteProgram(dynamic_pointer_cast<Program>(sampler.lock()->getProgram(pgm).lock()));
        }
        else
        {
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
		setPgm(pgm + i);
}

void DeleteProgramScreen::displayPgm()
{
    findField("pgm").lock()->setText(StrUtil::padLeft(to_string(pgm + 1), " ", 2) + "-" + dynamic_pointer_cast<Program>(sampler.lock()->getProgram(pgm).lock())->getName());
}

void DeleteProgramScreen::setPgm(int i)
{
    if (i < 0 || i >= sampler.lock()->getProgramCount())
        return;

    pgm = i;
    displayPgm();
}
