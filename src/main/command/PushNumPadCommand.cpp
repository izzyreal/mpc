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
                mpc.getLayeredScreen()->openScreen<VmpcSettingsScreen>();
                break;
            }
            case 1: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen<SongScreen>();
                break;
            }
            case 2: {
                mpc.getLayeredScreen()->openScreen<PunchScreen>();
                break;
            }
            case 3: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen<LoadScreen>();
                break;
            }
            case 4: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen<SampleScreen>();
                break;
            }
            case 5: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen<TrimScreen>();
                break;
            }
            case 6: {
                auto newDrum = mpc.getSequencer()->getActiveTrack()->getBus() - 1;
                if (newDrum >= 0)
                    mpc.screens->get<lcdgui::screens::DrumScreen>()->setDrum(newDrum);
                mpc.getLayeredScreen()->openScreen<SelectDrumScreen>();
                break;
            }
            case 7: {
                auto newDrum = mpc.getSequencer()->getActiveTrack()->getBus() - 1;
                if (newDrum >= 0)
                    mpc.screens->get<lcdgui::screens::DrumScreen>()->setDrum(newDrum);
                mpc.getLayeredScreen()->openScreen<SelectMixerDrumScreen>();
                break;
            }
            case 8: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen<OthersScreen>();
                break;
            }
            case 9: {
                if (!mpc.getSequencer()->isPlaying())
                    mpc.getLayeredScreen()->openScreen<SyncScreen>();
                break;
            }
        }

        return;
    }

    const auto fieldName = mpc.getLayeredScreen()->getFocusedFieldName();

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

