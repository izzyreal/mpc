#include "LoadAProgramScreen.hpp"

#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "lcdgui/screens/LoadScreen.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadAProgramScreen::LoadAProgramScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "load-a-program", layerIndex)
{
}

void LoadAProgramScreen::open()
{
    displayLoadReplaceSound();
}

void LoadAProgramScreen::turnWheel(const int i)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "load-replace-sound")
    {
        loadReplaceSameSound = i > 0;
        displayLoadReplaceSound();
    }
}

void LoadAProgramScreen::function(const int i)
{
    const auto selectedFile =
        mpc.screens->get<ScreenId::LoadScreen>()->getSelectedFile();

    switch (i)
    {
        case 2:
        {
            sampler->deleteAllPrograms(/*createDefaultProgram=*/true);
            sampler->deleteAllSamples();

            mpc.getDisk()->readPgm2(selectedFile, sampler->getProgram(0), 0);
            break;
        }
        case 3:
            openScreenById(ScreenId::LoadScreen);
            break;
        case 4:
        {
            const auto newProgram =
                sampler->createNewProgramAddFirstAvailableSlot().lock();

            for (int pi = 0; pi < 24; pi++)
            {
                if (sampler->getProgram(pi) == newProgram)
                {
                    mpc.getDisk()->readPgm2(selectedFile, newProgram, pi);
                    break;
                }
            }

            if (const auto track = sequencer->getSelectedTrack();
                sequencer::isDrumBusType(track->getBusType()))
            {
                for (int pgmIndex = 0;
                     pgmIndex < Mpc2000XlSpecs::MAX_PROGRAM_COUNT; pgmIndex++)
                {
                    if (sampler->getProgram(pgmIndex) == newProgram)
                    {
                        getActiveDrumBus()->setProgramIndex(
                            ProgramIndex(pgmIndex));
                        break;
                    }
                }
            }

            break;
        }
        default:;
    }
}

void LoadAProgramScreen::displayLoadReplaceSound() const
{
    findField("load-replace-sound")
        ->setText(std::string(loadReplaceSameSound ? "YES" : "NO(FASTER)"));
}

void LoadAProgramScreen::setLoadReplaceSameSound(const bool b)
{
    loadReplaceSameSound = b;
}
