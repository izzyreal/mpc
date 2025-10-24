#include "NoteInputScreenUpdateCommand.hpp"

#include "command/context/NoteInputScreenUpdateContext.hpp"

#include "lcdgui/screens/WithTimesAndNotes.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/window/ChannelSettingsScreen.hpp"
#include "lcdgui/screens/window/EditMultipleScreen.hpp"
#include "lcdgui/screens/StepEditorScreen.hpp"

#include "sequencer/NoteEvent.hpp"

#include <memory>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::command;
using namespace mpc::command::context;

NoteInputScreenUpdateCommand::NoteInputScreenUpdateCommand(NoteInputScreenUpdateContext &ctxToUse, const int noteToUse)
    : ctx(ctxToUse), note(noteToUse)
{
}

void NoteInputScreenUpdateCommand::execute()
{
    if (ctx.isSixteenLevelsEnabled)
    {
        return;
    }

    const auto screenComponent = ctx.screenComponent;

    if (mpc::sequencer::isDrumNote(note) && ctx.isAllowCentralNoteAndPadUpdateScreen)
    {
        ctx.setMpcNote(note);
    }
    else if (auto withNotes = std::dynamic_pointer_cast<WithTimesAndNotes>(screenComponent);
             withNotes && note >= 35)
    {
        withNotes->setNote0(note);
    }
    else if (auto assign16LevelsScreen = std::dynamic_pointer_cast<Assign16LevelsScreen>(screenComponent);
             assign16LevelsScreen)
    {
        assign16LevelsScreen->setNote(note);
    }
    else if (auto editMultipleScreen = std::dynamic_pointer_cast<EditMultipleScreen>(screenComponent);
             editMultipleScreen)
    {
        editMultipleScreen->setChangeNoteTo(note);
    }
    else if (auto stepEditorScreen = std::dynamic_pointer_cast<StepEditorScreen>(screenComponent);
             stepEditorScreen && ctx.currentFieldName == "fromnote" && note > 34)
    {
        stepEditorScreen->setFromNote(note);
    }
    else if (auto channelSettingsScreen = std::dynamic_pointer_cast<ChannelSettingsScreen>(screenComponent);
             channelSettingsScreen)
    {
        channelSettingsScreen->setNote(note);
    }
}
