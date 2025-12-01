#include "PasteEventScreen.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/StepEditorScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;

PasteEventScreen::PasteEventScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "paste-event", layerIndex)
{
}

void PasteEventScreen::function(const int i)
{
    ScreenComponent::function(i);

    if (i != 4)
    {
        return;
    }

    const auto stepEditorScreen =
        mpc.screens->get<ScreenId::StepEditorScreen>();

    if (stepEditorScreen->getPlaceHolder().empty())
    {
        openScreenById(ScreenId::StepEditorScreen);
        return;
    }

    for (int eventIndex = 0;
         eventIndex < stepEditorScreen->getPlaceHolder().size(); ++eventIndex)
    {
        const auto event = stepEditorScreen->getPlaceHolder()[eventIndex];
        constexpr bool allowMultipleNoteEventsWithSameNoteOnSameTick = true;
        sequencer::EventData eventState = *event->handle;
        eventState.tick = sequencer.lock()->getTransport()->getTickPosition();

        std::function onComplete = [] {};

        if (eventIndex == stepEditorScreen->getPlaceHolder().size() - 1)
        {
            onComplete = [ls = ls]
            {
                ls.lock()->postToUiThread(
                    [ls]
                    {
                        ls.lock()->openScreenById(ScreenId::StepEditorScreen);
                    });
            };
        }

        sequencer.lock()->getSelectedTrack()->insertEvent(
            eventState, allowMultipleNoteEventsWithSameNoteOnSameTick,
            onComplete);
    }
}
