#include "CreateNewProgramScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;

CreateNewProgramScreen::CreateNewProgramScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "create-new-program", layerIndex)
{
}

void CreateNewProgramScreen::turnWheel(int i)
{
	init();

	if (param == "midi-program-change")
    {
        setMidiProgramChange(midiProgramChange + i);
    }
    else if (param == "new-name")
    {
        auto nameScreen = mpc.screens->get<NameScreen>("name");
        auto createNewProgramScreen = this;
        nameScreen->setName(newName);

        auto renamer = [createNewProgramScreen](std::string& newName1) {
            createNewProgramScreen->newName = newName1;
        };

        nameScreen->setRenamerAndScreenToReturnTo(renamer, "create-new-program");
        openScreen("name");
    }
}

void CreateNewProgramScreen::open()
{
    if (ls->getPreviousScreenName() != "name")
    {
        auto letterIndex = 21 + 24;
        
        for (int i = 0; i < sampler->getPrograms().size(); i++)
        {
            if (!sampler->getProgram(i))
            {
                letterIndex = 21 + i;
                midiProgramChange = i + 1;
                break;
            }
        }
        
        newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterIndex];
    }
    
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
		auto newProgram = sampler->createNewProgramAddFirstAvailableSlot().lock();
		newProgram->setName(newName);
		newProgram->setMidiProgramChange(midiProgramChange);
		
		auto index = sampler->getProgramCount() - 1;

		for (int j = 0; j < sampler->getPrograms().size(); j++)
		{
			if (sampler->getProgram(j) == newProgram)
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
	findField("midi-program-change")->setTextPadded(midiProgramChange);
}

void CreateNewProgramScreen::displayNewName()
{
	findField("new-name")->setText(newName);
}

void CreateNewProgramScreen::setMidiProgramChange(int i)
{
	if (i < 1) i = 1;
    else if (i > 128) i = 128;

	if (midiProgramChange == i)
		return;

	midiProgramChange = i;
	displayMidiProgramChange();
}
