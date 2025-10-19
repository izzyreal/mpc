#include "PushNumPadCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"
#include "lcdgui/Field.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/LcdGuiUtil.h"
#include "sequencer/Track.hpp"

using namespace mpc::command;

PushNumPadCommand::PushNumPadCommand(mpc::Mpc &mpc, int i) : mpc(mpc), i(i)
{
}

void PushNumPadCommand::execute()
{
    if (mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
    {
        switch (i) {
            case 0: {
                mpc.getLayeredScreen()->openScreen("vmpc-settings");
                break;
            }
            case 1: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen("song");
                break;
            }
            case 2: {
                mpc.getLayeredScreen()->openScreen("punch");
                break;
            }
            case 3: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen("load");
                break;
            }
            case 4: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen("sample");
                break;
            }
            case 5: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen("trim");
                break;
            }
            case 6: {
                auto newDrum = mpc.getSequencer()->getActiveTrack()->getBus() - 1;
                if (newDrum >= 0)
                    mpc.screens->get<lcdgui::screens::DrumScreen>()->setDrum(newDrum);
                mpc.getLayeredScreen()->openScreen("select-drum");
                break;
            }
            case 7: {
                auto newDrum = mpc.getSequencer()->getActiveTrack()->getBus() - 1;
                if (newDrum >= 0)
                    mpc.screens->get<lcdgui::screens::DrumScreen>()->setDrum(newDrum);
                mpc.getLayeredScreen()->openScreen("select-mixer-drum");
                break;
            }
            case 8: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen("others");
                break;
            }
            case 9: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen("sync");
                break;
            }
        }

        return;
    }

    const auto fieldName = mpc.getLayeredScreen()->getFocus();

    const auto screenName = mpc.getLayeredScreen()->getCurrentScreenName();

    if (screenName == "sequencer" && fieldName.substr(0, 3) == "now" && mpc.getSequencer()->isPlaying())
    {
        return;
    }

    if (lcdgui::util::isTypableField(screenName, fieldName))
    {
        auto field = mpc.getLayeredScreen()->getFocusedField();

        if (!field->isTypeModeEnabled())
        {
            field->enableTypeMode();
        }
        
        field->type(i);
    }
}

