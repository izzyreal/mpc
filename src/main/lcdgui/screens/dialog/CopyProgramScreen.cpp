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
        const bool up = increment > 0;
        auto candidate = pgm0;
        for (int i = 0; i < std::abs(increment); ++i)
        {
            candidate = sampler->getUsedProgram(candidate, up);
        }
        setPgm0(candidate);
    }
    else if (focusedFieldName == "pgm1")
    {
        setPgm1(pgm1 + increment);
    }
}

void CopyProgramScreen::setPgm0(const ProgramIndex i)
{
    pgm0 = std::clamp(i, MinProgramIndex, MaxProgramIndex);
    displayPgm0();
    displayFunctionKeys();
}

void CopyProgramScreen::setPgm1(const ProgramIndex i)
{
    pgm1 = std::clamp(i, MinProgramIndex, MaxProgramIndex);
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

    const auto programName =
        program1->isUsed() ? program1->getName() : "(no program)";
    findField("pgm1")->setText(
        StrUtil::padLeft(std::to_string(pgm1 + 1), " ", 2) + "-" + programName);
}

void CopyProgramScreen::displayFunctionKeys()
{
    ls->setFunctionKeysArrangement(pgm0 == pgm1 ? 1 : 0);
    findBackground()->SetDirty();
}
