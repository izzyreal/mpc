#include "CopyProgramScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;

CopyProgramScreen::CopyProgramScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-program", layerIndex)
{
}

void CopyProgramScreen::open()
{
    pgm0 = activeDrum().getProgram();
    pgm1 = pgm0;

    displayPgm0();
    displayPgm1();
    displayFunctionKeys();
}

void CopyProgramScreen::function(int i)
{

    switch (i)

    {
        case int(3):
            mpc.getLayeredScreen()->openScreen<ProgramScreen>();
            break;
        case 4:
            if (pgm0 == pgm1)
            {
                return;
            }

            sampler->copyProgram(pgm0, pgm1);
            activeDrum().setProgram(pgm1);
            mpc.getLayeredScreen()->openScreen<ProgramScreen>();
            break;
    }
}

void CopyProgramScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "pgm0")
    {
        setPgm0(pgm0 + i);
    }
    else if (focusedFieldName == "pgm1")
    {
        setPgm1(pgm1 + i);
    }
}

void CopyProgramScreen::setPgm0(int i)
{
    auto candidate = i;
    auto up = i > pgm0;

    candidate = up ? candidate - 1 : candidate + 1;

    do
    {
        candidate = up ? candidate + 1 : candidate - 1;

        if (candidate < 0 || candidate >= sampler->getPrograms().size())
        {
            return;
        }
    } while (!sampler->getProgram(candidate));

    pgm0 = candidate;

    displayPgm0();
    displayFunctionKeys();
}

void CopyProgramScreen::setPgm1(int i)
{
    if (i < 0 || i >= sampler->getPrograms().size())
    {
        return;
    }

    pgm1 = i;
    displayPgm1();
    displayFunctionKeys();
}

void CopyProgramScreen::displayPgm0()
{
    auto programName = sampler->getProgram(pgm0)->getName();
    findField("pgm0")->setText(
        StrUtil::padLeft(std::to_string(pgm0 + 1), " ", 2) + "-" + programName);
}

void CopyProgramScreen::displayPgm1()
{
    auto program1 = sampler->getProgram(pgm1);

    auto programName = program1 ? program1->getName() : "(no program)";
    findField("pgm1")->setText(
        StrUtil::padLeft(std::to_string(pgm1 + 1), " ", 2) + "-" + programName);
}

void CopyProgramScreen::displayFunctionKeys()
{
    ls->setFunctionKeysArrangement(pgm0 == pgm1 ? 1 : 0);
    findBackground()->SetDirty();
}
