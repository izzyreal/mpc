#include "PasteEventScreen.hpp"

#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/StepEditorScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;

PasteEventScreen::PasteEventScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "paste-event", layerIndex)
{
}

void PasteEventScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
            const auto stepEditorScreen =
                mpc.screens->get<ScreenId::StepEditorScreen>();

            const bool allowMultipleNotesOnSameTick = true;

            for (auto &event : stepEditorScreen->getPlaceHolder())
            {
                sequencer->getActiveTrack()->cloneEventIntoTrack(
                    event, sequencer->getTickPosition(),
                    allowMultipleNotesOnSameTick);
            }
            openScreenById(ScreenId::StepEditorScreen);
            break;
    }
}
