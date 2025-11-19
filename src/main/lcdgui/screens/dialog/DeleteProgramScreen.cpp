#include "DeleteProgramScreen.hpp"

#include "StrUtil.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;

DeleteProgramScreen::DeleteProgramScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-program", layerIndex)
{
}

void DeleteProgramScreen::open()
{
    pgm = getActiveDrumBus()->getProgramIndex();
    displayPgm();
}

void DeleteProgramScreen::function(const int i)
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
        default:;
    }
}

void DeleteProgramScreen::turnWheel(const int i)
{

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "pgm")
    {
        setPgm(pgm + i);
    }
}

void DeleteProgramScreen::displayPgm() const
{
    findField("pgm")->setText(
        StrUtil::padLeft(std::to_string(pgm + 1), " ", 2) + "-" +
        sampler->getProgram(pgm)->getName());
}

void DeleteProgramScreen::setPgm(const int i)
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
    } while (!sampler->getProgram(candidate)->isUsed());

    pgm = candidate;
    displayPgm();
}
