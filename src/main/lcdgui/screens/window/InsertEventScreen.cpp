#include "InsertEventScreen.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "TimingCorrectScreen.hpp"

#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"

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
            openScreenById(ScreenId::StepEditorScreen);
            break;
    }
}

void InsertEventScreen::insertEvent() const
{
    const auto track = sequencer->getActiveTrack();

    if (insertEventType == 0)
    {
        constexpr bool allowMultipleNotesOnSameTick = true;
        const auto noteEvent = std::make_shared<NoteOnEvent>();
        track->addEvent(sequencer->getTransport()->getTickPosition(), noteEvent,
                        allowMultipleNotesOnSameTick);
        const auto timingCorrectScreen =
            mpc.screens->get<ScreenId::TimingCorrectScreen>();
        unsigned short duration =
            timingCorrectScreen->getNoteValueLengthInTicks();
        noteEvent->setDuration(duration);
        noteEvent->setNote(60);
        noteEvent->setVelocity(127);
        noteEvent->setVariationType(NoteOnEvent::VARIATION_TYPE::TUNE_0);
        noteEvent->setVariationValue(64);
    }
    else if (insertEventType == 1)
    {
        const auto pitchBendEvent = std::make_shared<PitchBendEvent>();
        track->addEvent(sequencer->getTransport()->getTickPosition(),
                        pitchBendEvent);
        pitchBendEvent->setAmount(0);
    }
    else if (insertEventType == 2)
    {
        const auto controlChangeEvent = std::make_shared<ControlChangeEvent>();
        track->addEvent(sequencer->getTransport()->getTickPosition(),
                        controlChangeEvent);
        controlChangeEvent->setController(0);
        controlChangeEvent->setAmount(0);
    }
    else if (insertEventType == 3)
    {
        const auto programChangeEvent = std::make_shared<ProgramChangeEvent>();
        track->addEvent(sequencer->getTransport()->getTickPosition(),
                        programChangeEvent);
        programChangeEvent->setProgram(1);
    }
    else if (insertEventType == 4)
    {
        const auto channelPressureEvent =
            std::make_shared<ChannelPressureEvent>();
        track->addEvent(sequencer->getTransport()->getTickPosition(),
                        channelPressureEvent);
        channelPressureEvent->setAmount(0);
    }
    else if (insertEventType == 5)
    {
        const auto polyPressureEvent = std::make_shared<PolyPressureEvent>();
        track->addEvent(sequencer->getTransport()->getTickPosition(),
                        polyPressureEvent);
        polyPressureEvent->setNote(60);
        polyPressureEvent->setAmount(0);
    }
    else if (insertEventType == 6)
    {
        const auto systemExclusiveEvent =
            std::make_shared<SystemExclusiveEvent>();
        track->addEvent(sequencer->getTransport()->getTickPosition(),
                        systemExclusiveEvent);
        systemExclusiveEvent->setByteB(247);
    }
    else if (insertEventType == 7)
    {
        const auto mixerEvent = std::make_shared<MixerEvent>();
        track->addEvent(sequencer->getTransport()->getTickPosition(),
                        mixerEvent);
        mixerEvent->setPadNumber(0);
        mixerEvent->setParameter(0);
        mixerEvent->setValue(0);
    }
}

void InsertEventScreen::turnWheel(const int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "eventtype")
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
