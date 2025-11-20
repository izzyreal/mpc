#include "NoteInputScreenUpdateCommand.hpp"

#include "command/context/NoteInputScreenUpdateContext.hpp"

#include "lcdgui/screens/WithTimesAndNotes.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/window/ChannelSettingsScreen.hpp"
#include "lcdgui/screens/window/EditMultipleScreen.hpp"
#include "lcdgui/screens/StepEditorScreen.hpp"

#include <memory>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::command;
using namespace mpc::command::context;

NoteInputScreenUpdateCommand::NoteInputScreenUpdateCommand(
    NoteInputScreenUpdateContext &ctxToUse, const NoteNumber noteToUse)
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

    if (isDrumNote(note) && ctx.isAllowCentralNoteAndPadUpdateScreen)
    {
        ctx.setSelectedNote(note);
    }
    else if (const auto withNotes =
                 std::dynamic_pointer_cast<WithTimesAndNotes>(screenComponent);
             withNotes && note >= MinDrumNoteNumber)
    {
        withNotes->setNote0(note);
    }
    else if (const auto assign16LevelsScreen =
                 std::dynamic_pointer_cast<Assign16LevelsScreen>(
                     screenComponent);
             assign16LevelsScreen)
    {
        assert(isDrumNote(note));
        assign16LevelsScreen->setNote(DrumNoteNumber(note));
    }
    else if (const auto editMultipleScreen =
                 std::dynamic_pointer_cast<EditMultipleScreen>(screenComponent);
             editMultipleScreen)
    {
        editMultipleScreen->setChangeNoteTo(note);
    }
    else if (const auto stepEditorScreen =
                 std::dynamic_pointer_cast<StepEditorScreen>(screenComponent);
             stepEditorScreen && ctx.currentFieldNameIsFromNote &&
             note > NoDrumNoteAssigned && note <= MaxDrumNoteNumber)
    {
        stepEditorScreen->setFromNote(DrumNoteNumber(note));
    }
    else if (const auto channelSettingsScreen =
                 std::dynamic_pointer_cast<ChannelSettingsScreen>(
                     screenComponent);
             channelSettingsScreen)
    {
        channelSettingsScreen->setNote(note);
    }
}
