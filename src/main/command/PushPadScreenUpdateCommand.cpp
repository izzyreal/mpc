#include "PushPadScreenUpdateCommand.hpp"
#include "sequencer/Transport.hpp"
#include "command/context/PushPadScreenUpdateContext.hpp"

#include "lcdgui/screens/AssignScreen.hpp"
#include "lcdgui/screens/MixerScreen.hpp"
#include "lcdgui/screens/NextSeqPadScreen.hpp"
#include "lcdgui/screens/TrMuteScreen.hpp"
#include "sampler/Program.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

#include <memory>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::command;
using namespace mpc::command::context;

PushPadScreenUpdateCommand::PushPadScreenUpdateCommand(
    PushPadScreenUpdateContext &ctxToUse, int padIndexWithBankToUse)
    : ctx(ctxToUse), padIndexWithBank(padIndexWithBankToUse)
{
}

void PushPadScreenUpdateCommand::execute()
{
    if (ctx.isSixteenLevelsEnabled)
    {
        return;
    }

    const auto screenComponent = ctx.screenComponent;

    if (ctx.isAllowCentralNoteAndPadUpdateScreen)
    {
        ctx.setSelectedPad(padIndexWithBank);
    }
    else if (auto mixerScreen =
                 std::dynamic_pointer_cast<MixerScreen>(screenComponent);
             mixerScreen)
    {
        const unsigned char bankStartPadIndex = static_cast<int>(ctx.bank) * 16;
        const unsigned char bankEndPadIndex = bankStartPadIndex + 16;

        if (padIndexWithBank >= bankStartPadIndex &&
            padIndexWithBank < bankEndPadIndex)
        {
            mixerScreen->pressPadIndexWithoutBank(padIndexWithBank % 16);
        }
    }
    else if (auto trMuteScreen =
                 std::dynamic_pointer_cast<TrMuteScreen>(screenComponent);
             trMuteScreen)
    {
        if (!ctx.sequencer)
        {
            return;
        }

        if (ctx.isF6Pressed || ctx.sequencer->isSoloEnabled())
        {
            if (!ctx.sequencer->isSoloEnabled())
            {
                ctx.sequencer->setSoloEnabled(true);
            }

            ctx.sequencer->setActiveTrackIndex(padIndexWithBank);
            trMuteScreen->findBackground()->setBackgroundName(
                "track-mute-solo-2");
        }
        else
        {
            const auto s = ctx.sequencer->getActiveSequence();
            const auto t = s->getTrack(padIndexWithBank);
            t->setOn(!t->isOn());
        }
    }
    else if (auto nextSeqPadScreen =
                 std::dynamic_pointer_cast<NextSeqPadScreen>(screenComponent);
             nextSeqPadScreen)
    {
        if (!ctx.sequencer)
        {
            return;
        }

        if (ctx.sequencer->getTransport()->isPlaying() && ctx.isF4Pressed)
        {
            if (!ctx.sequencer->getSequence(padIndexWithBank)->isUsed())
            {
                return;
            }

            ctx.sequencer->getStateManager()->enqueue(sequencer::SwitchToNextSequence{padIndexWithBank});
            return;
        }

        ctx.sequencer->setNextSqPad(padIndexWithBank);
    }
    else if (auto assignScreen =
                 std::dynamic_pointer_cast<AssignScreen>(screenComponent);
             assignScreen)
    {
        const auto nn = ctx.program->getNoteFromPad(padIndexWithBank);
        ctx.program->getSlider()->setAssignNote(nn);
    }
}
