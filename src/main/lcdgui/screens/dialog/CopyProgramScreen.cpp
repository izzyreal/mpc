#include "CopyProgramScreen.hpp"

#include "StrUtil.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens::dialog;

CopyProgramScreen::CopyProgramScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-program", layerIndex)
{
}

void CopyProgramScreen::open()
{
    pgm0 = getActiveDrumBus()->getProgramIndex();
    pgm1 = pgm0;

    displayPgm0();
    displayPgm1();
    displayFunctionKeys();
}

void CopyProgramScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::ProgramScreen);
            break;
        case 4:
            if (pgm0 == pgm1)
            {
                return;
            }

            sampler->copyProgram(pgm0, pgm1);
            getActiveDrumBus()->setProgramIndex(pgm1);
            openScreenById(ScreenId::ProgramScreen);
            break;
        default:;
    }
}

void CopyProgramScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "pgm0")
    {
        setPgm0(pgm0 + increment);
    }
    else if (focusedFieldName == "pgm1")
    {
        setPgm1(pgm1 + increment);
    }
}

void CopyProgramScreen::setPgm0(const ProgramIndex i)
{
    ProgramIndex candidate = i;
    const auto up = i > pgm0;

    candidate = up ? candidate - 1 : candidate + 1;

    do
    {
        candidate = up ? candidate + 1 : candidate - 1;

        if (candidate < MinProgramIndex || candidate > MaxProgramIndex)
        {
            return;
        }
    } while (!sampler->getProgram(candidate));

    pgm0 = candidate;

    displayPgm0();
    displayFunctionKeys();
}

void CopyProgramScreen::setPgm1(const ProgramIndex i)
{
    if (i < 0 || i > MaxProgramIndex)
    {
        return;
    }

    pgm1 = i;
    displayPgm1();
    displayFunctionKeys();
}

void CopyProgramScreen::displayPgm0() const
{
    const auto programName = sampler->getProgram(pgm0)->getName();
    findField("pgm0")->setText(
        StrUtil::padLeft(std::to_string(pgm0 + 1), " ", 2) + "-" + programName);
}

void CopyProgramScreen::displayPgm1() const
{
    const auto program1 = sampler->getProgram(pgm1);

    const auto programName = program1 ? program1->getName() : "(no program)";
    findField("pgm1")->setText(
        StrUtil::padLeft(std::to_string(pgm1 + 1), " ", 2) + "-" + programName);
}

void CopyProgramScreen::displayFunctionKeys()
{
    ls->setFunctionKeysArrangement(pgm0 == pgm1 ? 1 : 0);
    findBackground()->SetDirty();
}
