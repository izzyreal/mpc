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
        openScreen("delete-all-programs");
        break;
    case 3:
        openScreen("program");
        break;
    case 4:
        if (sampler.lock()->getProgramCount() > 1)
        {
			sampler.lock()->deleteProgram(sampler.lock()->getProgram(pgm));
        }
        else
        {
			const bool initPgms = true;
			sampler.lock()->deleteAllPrograms(initPgms);
		}
        
        openScreen("program");
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
    findField("pgm").lock()->setText(StrUtil::padLeft(to_string(pgm + 1), " ", 2) + "-" + sampler.lock()->getProgram(pgm).lock()->getName());
}

void DeleteProgramScreen::setPgm(int i)
{
    auto candidate = i;
    auto up = i > pgm;

    candidate = up ? candidate - 1 : candidate + 1;

    do {
        candidate = up ? candidate + 1 : candidate - 1;

        if (candidate < 0 || candidate >= sampler.lock()->getPrograms().size())
            return;
    } while (!sampler.lock()->getProgram(candidate).lock());

    pgm = candidate;
    displayPgm();
}
