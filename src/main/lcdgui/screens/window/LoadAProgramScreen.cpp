#include "LoadAProgramScreen.hpp"

#include "disk/AbstractDisk.hpp"
#include "lcdgui/screens/LoadScreen.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadAProgramScreen::LoadAProgramScreen(mpc::Mpc &mpc, const int layerIndex)
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

    auto selectedFile =
        mpc.screens->get<ScreenId::LoadScreen>()->getSelectedFile();

    switch (i)
    {
        case 2:
        {
            sampler->deleteAllPrograms(/*createDefaultProgram=*/true);
            sampler->deleteAllSamples();

            mpc.getDisk()->readPgm2(selectedFile, sampler->getProgram(0));
            break;
        }
        case 3:
            openScreenById(ScreenId::LoadScreen);
            break;
        case 4:
        {
            auto newProgram =
                sampler->createNewProgramAddFirstAvailableSlot().lock();

            mpc.getDisk()->readPgm2(selectedFile, newProgram);

            auto track = sequencer->getActiveTrack();

            if (track->getBus() > 0)
            {
                for (int pgmIndex = 0; pgmIndex < 24; pgmIndex++)
                {
                    if (sampler->getProgram(pgmIndex) == newProgram)
                    {
                        getActiveDrumBus()->setProgram(pgmIndex);
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
    findField("load-replace-sound")
        ->setText(std::string(loadReplaceSameSound ? "YES" : "NO(FASTER)"));
}

void LoadAProgramScreen::setLoadReplaceSameSound(bool b)
{
    loadReplaceSameSound = b;
}
