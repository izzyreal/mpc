#include "CreateNewProgramScreen.hpp"

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::controls;
using namespace std;

CreateNewProgramScreen::CreateNewProgramScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "create-new-program", layerIndex)
{
}

void CreateNewProgramScreen::turnWheel(int i)
{
	init();

	if (param.compare("midi-program-change") == 0)
		setMidiProgramChange(midiProgramChange + i);
}

void CreateNewProgramScreen::open()
{
	auto letterIndex = 21 + 24;

	for (int i = 0; i < sampler.lock()->getPrograms().size(); i++)
	{
		if (!sampler.lock()->getProgram(i).lock())
		{
			letterIndex = 21 + i;
			midiProgramChange = i + 1;
			break;
		}
	}

	newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterIndex];
	
	init();

	displayNewName();
	displayMidiProgramChange();
}

void CreateNewProgramScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		openScreen("program");
		break;
	case 4:
		auto newProgram = sampler.lock()->addProgram().lock();
		newProgram->setName(newName);
		newProgram->setMidiProgramChange(midiProgramChange);
		
		auto index = sampler.lock()->getProgramCount() - 1;

		for (int j = 0; j < sampler.lock()->getPrograms().size(); j++)
		{
			if (sampler.lock()->getProgram(j).lock() == newProgram)
			{
				index = j;
				break;
			}
		}

		mpcSoundPlayerChannel->setProgram(index);
		openScreen("program");
		break;
	}
}

void CreateNewProgramScreen::displayMidiProgramChange()
{
	findField("midi-program-change").lock()->setTextPadded(midiProgramChange);
}

void CreateNewProgramScreen::displayNewName()
{
	findField("new-name").lock()->setText(newName);
}

void CreateNewProgramScreen::setMidiProgramChange(int i)
{
	if (i < 1) i = 1;
    if (i > 128) i = 128;

	if (midiProgramChange == i)
		return;

	midiProgramChange = i;
	displayMidiProgramChange();
}
