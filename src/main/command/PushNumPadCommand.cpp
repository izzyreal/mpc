#include "PushNumPadCommand.hpp"
#include "Mpc.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/Field.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/LcdGuiUtil.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushNumPadCommand::PushNumPadCommand(mpc::Mpc &mpc, int i) : mpc(mpc), i(i) {}

void PushNumPadCommand::execute()
{
    if (mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
    {
        switch (i)
        {
            case 0:
            {
                mpc.getLayeredScreen()->openScreenById(
                    ScreenId::VmpcSettingsScreen);
                break;
            }
            case 1:
            {
                if (!mpc.getSequencer()->isPlaying())
                {
                    mpc.getLayeredScreen()->openScreenById(
                        ScreenId::SongScreen);
                }
                break;
            }
            case 2:
            {
                mpc.getLayeredScreen()->openScreenById(ScreenId::PunchScreen);
                break;
            }
            case 3:
            {
                if (!mpc.getSequencer()->isPlaying())
                {
                    mpc.getLayeredScreen()->openScreenById(
                        ScreenId::LoadScreen);
                }
                break;
            }
            case 4:
            {
                if (!mpc.getSequencer()->isPlaying())
                {
                    mpc.getLayeredScreen()->openScreenById(
                        ScreenId::SampleScreen);
                }
                break;
            }
            case 5:
            {
                if (!mpc.getSequencer()->isPlaying())
                {
                    mpc.getLayeredScreen()->openScreenById(
                        ScreenId::TrimScreen);
                }
                break;
            }
            case 6:
            {
                auto newDrum =
                    mpc.getSequencer()->getActiveTrack()->getBus() - 1;
                if (newDrum >= 0)
                {
                    mpc.screens->get<ScreenId::DrumScreen>()->setDrum(newDrum);
                }
                mpc.getLayeredScreen()->openScreenById(
                    ScreenId::SelectDrumScreen);
                break;
            }
            case 7:
            {
                auto newDrum =
                    mpc.getSequencer()->getActiveTrack()->getBus() - 1;
                if (newDrum >= 0)
                {
                    mpc.screens->get<ScreenId::DrumScreen>()->setDrum(newDrum);
                }
                mpc.getLayeredScreen()->openScreenById(
                    ScreenId::SelectMixerDrumScreen);
                break;
            }
            case 8:
            {
                if (!mpc.getSequencer()->isPlaying())
                {
                    mpc.getLayeredScreen()->openScreenById(
                        ScreenId::OthersScreen);
                }
                break;
            }
            case 9:
            {
                if (!mpc.getSequencer()->isPlaying())
                {
                    mpc.getLayeredScreen()->openScreenById(
                        ScreenId::SyncScreen);
                }
                break;
            }
        }

        return;
    }

    const auto fieldName = mpc.getLayeredScreen()->getFocusedFieldName();

    const auto screenName = mpc.getLayeredScreen()->getCurrentScreenName();

    if (screenName == "sequencer" && fieldName.substr(0, 3) == "now" &&
        mpc.getSequencer()->isPlaying())
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
