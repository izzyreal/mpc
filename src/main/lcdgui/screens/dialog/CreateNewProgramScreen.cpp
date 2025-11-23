#include "CreateNewProgramScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;

CreateNewProgramScreen::CreateNewProgramScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "create-new-program", layerIndex)
{
}

void CreateNewProgramScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "midi-program-change")
    {
        setMidiProgramChange(midiProgramChange + increment);
    }
}

void CreateNewProgramScreen::openNameScreen()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "new-name")
    {
        const auto enterAction = [this](const std::string &nameScreenName)
        {
            newName = nameScreenName;
            openScreenById(ScreenId::CreateNewProgramScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(newName, 16, enterAction, "create-new-program");
        openScreenById(ScreenId::NameScreen);
    }
}

void CreateNewProgramScreen::open()
{
    if (ls.lock()->isPreviousScreenNot({ScreenId::NameScreen}))
    {
        auto letterIndex = 21 + 24;

        for (int i = 0; i < sampler.lock()->getPrograms().size(); i++)
        {
            if (!sampler.lock()->getProgram(i)->isUsed())
            {
                letterIndex = 21 + i;
                midiProgramChange = i + 1;
                break;
            }
        }

        newName = "NewPgm-" + Mpc::akaiAscii[letterIndex];
    }

    displayNewName();
    displayMidiProgramChange();
}

void CreateNewProgramScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::ProgramScreen);
            break;
        case 4:
        {
            const auto newProgram =
                sampler.lock()->createNewProgramAddFirstAvailableSlot().lock();
            newProgram->setName(newName);
            newProgram->setMidiProgramChange(midiProgramChange);

            auto index = ProgramIndex(sampler.lock()->getProgramCount() - 1);

            for (int j = 0; j < sampler.lock()->getPrograms().size(); j++)
            {
                if (sampler.lock()->getProgram(j) == newProgram)
                {
                    index = ProgramIndex(j);
                    break;
                }
            }

            getActiveDrumBus()->setProgramIndex(index);
            openScreenById(ScreenId::ProgramScreen);
            break;
        }
        default:;
    }
}

void CreateNewProgramScreen::displayMidiProgramChange() const
{
    findField("midi-program-change")->setTextPadded(midiProgramChange);
}

void CreateNewProgramScreen::displayNewName() const
{
    findField("new-name")->setText(newName);
}

void CreateNewProgramScreen::setMidiProgramChange(int i)
{
    if (i < 1)
    {
        i = 1;
    }
    else if (i > 128)
    {
        i = 128;
    }

    if (midiProgramChange == i)
    {
        return;
    }

    midiProgramChange = i;
    displayMidiProgramChange();
}
