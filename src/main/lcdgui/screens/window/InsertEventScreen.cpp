#include "InsertEventScreen.hpp"

#include "Mpc.hpp"

#include "sequencer/Transport.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sequencer/EventData.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

InsertEventScreen::InsertEventScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "insert-event", layerIndex)
{
}

void InsertEventScreen::open()
{
    eventAddedBeforeLeavingTheScreen = false;
    displayInsertEventType();
}

void InsertEventScreen::displayInsertEventType() const
{
    findField("eventtype")->setText(eventTypeNames[insertEventType]);
}

void InsertEventScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
            insertEvent();
            eventAddedBeforeLeavingTheScreen = true;
            break;
        default:;
    }
}

void InsertEventScreen::insertEvent() const
{
    EventData e;
    e.tick = sequencer.lock()->getTransport()->getTickPosition();

    if (insertEventType == 0)
    {
        e.type = EventType::NoteOn;
        const auto timingCorrectScreen =
            mpc.screens->get<ScreenId::TimingCorrectScreen>();

        e.duration = Duration(timingCorrectScreen->getNoteValueLengthInTicks());
        e.noteNumber = NoteNumber(60);
        e.velocity = MaxVelocity;
        e.noteVariationType = NoteVariationTypeTune;
        e.noteVariationValue = DefaultNoteVariationValue;
    }
    else if (insertEventType == 1)
    {
        e.type = EventType::PitchBend;
        e.amount = 0;
    }
    else if (insertEventType == 2)
    {
        e.type = EventType::ControlChange;
        e.controllerNumber = 0;
        e.controllerValue = 0;
    }
    else if (insertEventType == 3)
    {
        e.type = EventType::ProgramChange;
        e.programChangeProgramIndex = ProgramIndex(1);
    }
    else if (insertEventType == 4)
    {
        e.type = EventType::ChannelPressure;
        e.amount = 0;
    }
    else if (insertEventType == 5)
    {
        e.type = EventType::PolyPressure;
        e.noteNumber = NoteNumber(60);
        e.amount = 0;
    }
    else if (insertEventType == 6)
    {
        e.type = EventType::SystemExclusive;
    }
    else if (insertEventType == 7)
    {
        e.type = EventType::Mixer;
        e.mixerPad = 0;
        e.mixerParameter = 0;
        e.mixerValue = 0;
    }

    utils::SimpleAction onComplete(
        [ls = ls]
        {
            ls.lock()->postToUiThread(utils::Task(
                [ls]
                {
                    ls.lock()->openScreenById(ScreenId::StepEditorScreen);
                }));
        });

    sequencer.lock()->getSelectedTrack()->acquireAndInsertEvent(
        e, std::move(onComplete));
}

void InsertEventScreen::turnWheel(const int i)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "eventtype")
    {
        setInsertEventType(insertEventType + i);
    }
}

void InsertEventScreen::setInsertEventType(const int i)
{
    insertEventType = std::clamp(i, 0, 7);
    displayInsertEventType();
}

bool InsertEventScreen::isEventAddedBeforeLeavingTheScreen() const
{
    return eventAddedBeforeLeavingTheScreen;
}
