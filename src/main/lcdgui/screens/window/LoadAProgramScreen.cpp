#include "LoadAProgramScreen.hpp"

#include <disk/AbstractDisk.hpp>
#include <lcdgui/screens/LoadScreen.hpp>

#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadAProgramScreen::LoadAProgramScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "load-a-program", layerIndex)
{
}

void LoadAProgramScreen::open()
{
	displayLoadReplaceSound();
}

void LoadAProgramScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

	if (focusedFieldName == "load-replace-sound")
	{
        loadReplaceSameSound = i > 0;
		displayLoadReplaceSound();
	}
}

void LoadAProgramScreen::function(int i)
{

    auto selectedFile = mpc.screens->get<LoadScreen>()->getSelectedFile();
    
	switch (i)
	{
	case 2:
	{
        mpc.getSampler()->deleteAllPrograms(/*createDefaultProgram=*/true);
        mpc.getSampler()->deleteAllSamples();

        mpc.getDisk()->readPgm2(selectedFile, mpc.getSampler()->getProgram(0));
		break;
	}
	case 3:
        mpc.getLayeredScreen()->openScreen<LoadScreen>();
		break;
	case 4: {
        auto newProgram = mpc.getSampler()->createNewProgramAddFirstAvailableSlot().lock();

        mpc.getDisk()->readPgm2(selectedFile, newProgram);

        auto track = mpc.getSequencer()->getActiveTrack();

        if (track->getBus() > 0)
        {
            for (int pgmIndex = 0; pgmIndex < 24; pgmIndex++)
            {
                if (sampler->getProgram(pgmIndex) == newProgram)
                {
                    activeDrum().setProgram(pgmIndex);
                    break;
                }
            }
        }

        break;
    }
	}
}

void LoadAProgramScreen::displayLoadReplaceSound()
{
	findField("load-replace-sound")->setText(std::string(loadReplaceSameSound ? "YES" : "NO(FASTER)"));
}

void LoadAProgramScreen::setLoadReplaceSameSound(bool b)
{
    loadReplaceSameSound = b;
}

