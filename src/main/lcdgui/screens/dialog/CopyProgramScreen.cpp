#include "CopyProgramScreen.hpp"

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;

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

		sampler->copyProgram(pgm0, pgm1);
		mpcSoundPlayerChannel->setProgram(pgm1);
		openScreen("program");
		break;
	}
}

void CopyProgramScreen::turnWheel(int i)
{
	init();
		
	if (param == "pgm0")
		setPgm0(pgm0 + i);
	else if (param == "pgm1")
		setPgm1(pgm1 + i);
}

void CopyProgramScreen::setPgm0(int i)
{	
	auto candidate = i;
	auto up = i > pgm0;

	candidate = up ? candidate - 1 : candidate + 1;

	do {
		candidate = up ? candidate + 1 : candidate - 1;

		if (candidate < 0 || candidate >= sampler->getPrograms().size())
			return;
	}
	while (!sampler->getProgram(candidate).lock());
	
	pgm0 = candidate;

	displayPgm0();
	displayFunctionKeys();
}

void CopyProgramScreen::setPgm1(int i)
{
	if (i < 0 || i >= sampler->getPrograms().size())
		return;

	pgm1 = i;
	displayPgm1();
	displayFunctionKeys();
}

void CopyProgramScreen::displayPgm0()
{
	auto programName = sampler->getProgram(pgm0).lock()->getName();
	findField("pgm0")->setText(StrUtil::padLeft(std::to_string(pgm0 + 1), " ", 2) + "-" + programName);
}

void CopyProgramScreen::displayPgm1()
{
	auto program1 = sampler->getProgram(pgm1).lock();

	auto programName = program1 ? program1->getName() : "(no program)";
	findField("pgm1")->setText(StrUtil::padLeft(std::to_string(pgm1 + 1), " ", 2) + "-" + programName);
}

void CopyProgramScreen::displayFunctionKeys()
{
	ls.lock()->setFunctionKeysArrangement(pgm0 == pgm1 ? 1 : 0);
	findBackground()->SetDirty();
}
