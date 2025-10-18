#include "PushPadScreenUpdateCommand.h"

#include "command/context/PushPadScreenUpdateContext.h"

#include "lcdgui/screens/MixerScreen.hpp"
#include "lcdgui/screens/StepEditorScreen.hpp"
#include "lcdgui/screens/WithTimesAndNotes.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/window/ChannelSettingsScreen.hpp"
#include "lcdgui/screens/window/EditMultipleScreen.hpp"
#include "sequencer/NoteEvent.hpp"

using namespace mpc::command;
using namespace mpc::command::context;

PushPadScreenUpdateCommand::PushPadScreenUpdateCommand(PushPadScreenUpdateContext &ctxToUse, const int noteToUse, const std::optional<int> padIndexWithBankToUse)
    : ctx(ctxToUse), note(noteToUse), padIndexWithBank(padIndexWithBankToUse)
{
}

void PushPadScreenUpdateCommand::execute()
{
    if (ctx.isSixteenLevelsEnabled)
    {
        return;
    }

    const auto screenComponent = ctx.screenComponent;

    if (mpc::sequencer::isDrumNote(note) && ctx.isAllowCentralNoteAndPadUpdateScreen)
    {
        ctx.setMpcNote(note);

        if (padIndexWithBank)
        {
            ctx.setMpcPad(*padIndexWithBank);
        }
    }
    else if (auto withNotes = std::dynamic_pointer_cast<lcdgui::screens::WithTimesAndNotes>(screenComponent);
             withNotes && note >= 35)
    {
        withNotes->setNote0(note);
    }
    else if (auto assign16LevelsScreen = std::dynamic_pointer_cast<lcdgui::screens::window::Assign16LevelsScreen>(screenComponent);
             assign16LevelsScreen)
    {
        assign16LevelsScreen->setNote(note);
    }
    else if (auto editMultipleScreen = std::dynamic_pointer_cast<lcdgui::screens::window::EditMultipleScreen>(screenComponent);
             editMultipleScreen)
    {
        editMultipleScreen->setChangeNoteTo(note);
    }
    else if (auto stepEditorScreen = std::dynamic_pointer_cast<lcdgui::screens::StepEditorScreen>(screenComponent);
             stepEditorScreen && ctx.currentFieldName == "fromnote" && note > 34)
    {
        stepEditorScreen->setFromNote(note);
    }
    else if (auto mixerScreen = std::dynamic_pointer_cast<lcdgui::screens::MixerScreen>(screenComponent);
             mixerScreen && padIndexWithBank)
    {
        unsigned char bankStartPadIndex = ctx.bank * 16;
        unsigned char bankEndPadIndex = bankStartPadIndex + 16;

        if (*padIndexWithBank >= bankStartPadIndex && *padIndexWithBank < bankEndPadIndex)
        {
            mixerScreen->pressPadIndexWithoutBank(*padIndexWithBank % 16);
        }
    }
    else if (auto channelSettingsScreen = std::dynamic_pointer_cast<lcdgui::screens::window::ChannelSettingsScreen>(screenComponent);
             channelSettingsScreen)
    {
        channelSettingsScreen->setNote(note);
    }
}

