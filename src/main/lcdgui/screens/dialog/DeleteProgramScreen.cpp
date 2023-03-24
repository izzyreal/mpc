#include "DeleteProgramScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;
using namespace moduru::lang;

DeleteProgramScreen::DeleteProgramScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-program", layerIndex)
{
}

void DeleteProgramScreen::open()
{
    auto allPrograms = sampler->getPrograms();

    init();

    pgm = activeDrum().getProgram();

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
        if (sampler->getProgramCount() > 1)
        {
			sampler->deleteProgram(sampler->getProgram(pgm));
        }
        else
        {
			sampler->deleteAllPrograms();
            sampler->createNewProgramAddFirstAvailableSlot().lock()->setName("NewPgm-A");
		}
        
        openScreen("program");
        break;
    }
}

void DeleteProgramScreen::turnWheel(int i)
{
    init();

    if (param == "pgm")
		setPgm(pgm + i);
}

void DeleteProgramScreen::displayPgm()
{
    findField("pgm")->setText(StrUtil::padLeft(std::to_string(pgm + 1), " ", 2) + "-" + sampler->getProgram(pgm)->getName());
}

void DeleteProgramScreen::setPgm(int i)
{
    auto candidate = i;
    auto up = i > pgm;

    candidate = up ? candidate - 1 : candidate + 1;

    do {
        candidate = up ? candidate + 1 : candidate - 1;

        if (candidate < 0 || candidate >= sampler->getPrograms().size())
            return;
    } while (!sampler->getProgram(candidate));

    pgm = candidate;
    displayPgm();
}
