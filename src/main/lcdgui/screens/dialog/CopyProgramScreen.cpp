#include "CopyProgramScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;
using namespace std;

CopyProgramScreen::CopyProgramScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "copy-program", layerIndex)
{
}

void CopyProgramScreen::open()
{
	displayPgm0();
	displayPgm1();
}

void CopyProgramScreen::function(int i)
{
	init();
	
	switch (i)
	
	{
	case int(3) :
		ls.lock()->openScreen("program");
		break;
	case 4:
		if (pgm0 == pgm1) {
			return;
		}

		ls.lock()->openScreen("program");
		break;
	}
}

void CopyProgramScreen::turnWheel(int i)
{
	init();
		
	if (param.compare("pgm0") == 0)
	{
		setPgm0(pgm0 + i);
	}
	else if (param.compare("pgm1") == 0) {
		setPgm1(pgm1 + i);
	}
}

void CopyProgramScreen::setPgm0(int i)
{
	if (i < 0 || i >= sampler.lock()->getProgramCount())
	{
		return;
	}

	pgm0 = i;
	displayPgm0();
}

void CopyProgramScreen::setPgm1(int i)
{
	if (i < 0 || i >= sampler.lock()->getProgramCount())
	{
		return;
	}
	pgm1 = i;
	displayPgm1();
}

void CopyProgramScreen::displayPgm0()
{
	auto programName = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(pgm0).lock())->getName();
	findField("pgm0").lock()->setText(StrUtil::padLeft(to_string(pgm0 + 1), " ", 2) + "-" + programName);
}

void CopyProgramScreen::displayPgm1()
{
	auto programName = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(pgm1).lock())->getName();
	findField("pgm1").lock()->setText(StrUtil::padLeft(to_string(pgm1 + 1), " ", 2) + "-" + programName);
}
