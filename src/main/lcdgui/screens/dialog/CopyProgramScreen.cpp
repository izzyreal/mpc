#include "CopyProgramScreen.hpp"

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;
using namespace std;

CopyProgramScreen::CopyProgramScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "copy-program", layerIndex)
{
}

void CopyProgramScreen::open()
{
	init();
	pgm0 = mpcSoundPlayerChannel->getProgram();
	pgm1 = pgm0;

	displayPgm0();
	displayPgm1();
	displayFunctionKeys();
}

void CopyProgramScreen::function(int i)
{
	init();
	
	switch (i)
	
	{
	case int(3) :
		openScreen("program");
		break;
	case 4:
		if (pgm0 == pgm1)
			return;

		sampler.lock()->copyProgram(pgm0, pgm1);
		mpcSoundPlayerChannel->setProgram(pgm1);
		openScreen("program");
		break;
	}
}

void CopyProgramScreen::turnWheel(int i)
{
	init();
		
	if (param.compare("pgm0") == 0)
		setPgm0(pgm0 + i);
	else if (param.compare("pgm1") == 0)
		setPgm1(pgm1 + i);
}

void CopyProgramScreen::setPgm0(int i)
{	
	auto candidate = i;
	auto up = i > pgm0;

	candidate = up ? candidate - 1 : candidate + 1;

	do {
		candidate = up ? candidate + 1 : candidate - 1;

		if (candidate < 0 || candidate >= sampler.lock()->getPrograms().size())
			return;
	}
	while (!sampler.lock()->getProgram(candidate).lock());
	
	pgm0 = candidate;

	displayPgm0();
	displayFunctionKeys();
}

void CopyProgramScreen::setPgm1(int i)
{
	if (i < 0 || i >= sampler.lock()->getPrograms().size())
		return;

	pgm1 = i;
	displayPgm1();
	displayFunctionKeys();
}

void CopyProgramScreen::displayPgm0()
{
	auto programName = sampler.lock()->getProgram(pgm0).lock()->getName();
	findField("pgm0").lock()->setText(StrUtil::padLeft(to_string(pgm0 + 1), " ", 2) + "-" + programName);
}

void CopyProgramScreen::displayPgm1()
{
	auto program1 = sampler.lock()->getProgram(pgm1).lock();

	auto programName = program1 ? program1->getName() : "(no program)";
	findField("pgm1").lock()->setText(StrUtil::padLeft(to_string(pgm1 + 1), " ", 2) + "-" + programName);
}

void CopyProgramScreen::displayFunctionKeys()
{
	ls.lock()->setFunctionKeysArrangement(pgm0 == pgm1 ? 1 : 0);
	findBackground().lock()->SetDirty();
}
