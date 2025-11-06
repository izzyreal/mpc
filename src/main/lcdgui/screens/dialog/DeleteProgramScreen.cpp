#include "DeleteProgramScreen.hpp"

#include "StrUtil.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;

DeleteProgramScreen::DeleteProgramScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-program", layerIndex)
{
}

void DeleteProgramScreen::open()
{
    auto allPrograms = sampler->getPrograms();

    pgm = getActiveDrumBus()->getProgram();

    displayPgm();
}

void DeleteProgramScreen::function(int i)
{
    switch (i)
    {
        case 2:
            openScreenById(ScreenId::DeleteAllProgramsScreen);
            break;
        case 3:
            openScreenById(ScreenId::ProgramScreen);
            break;
        case 4:
            if (sampler->getProgramCount() > 1)
            {
                sampler->deleteProgram(sampler->getProgram(pgm));
            }
            else
            {
                sampler->deleteAllPrograms(/*createDefaultProgram=*/true);
            }

            openScreenById(ScreenId::ProgramScreen);
            break;
    }
}

void DeleteProgramScreen::turnWheel(int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "pgm")
    {
        setPgm(pgm + i);
    }
}

void DeleteProgramScreen::displayPgm()
{
    findField("pgm")->setText(
        StrUtil::padLeft(std::to_string(pgm + 1), " ", 2) + "-" +
        sampler->getProgram(pgm)->getName());
}

void DeleteProgramScreen::setPgm(int i)
{
    auto candidate = i;
    const auto up = i > pgm;

    candidate = up ? candidate - 1 : candidate + 1;

    do
    {
        candidate = up ? candidate + 1 : candidate - 1;

        if (candidate < 0 || candidate >= sampler->getPrograms().size())
        {
            return;
        }
    } while (!sampler->getProgram(candidate));

    pgm = candidate;
    displayPgm();
}
