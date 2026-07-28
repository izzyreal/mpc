#include "AuditionProgramNoteCommand.hpp"

#include "Mpc.hpp"
#include "audiomidi/EventHandler.hpp"
#include "performance/PerformanceManager.hpp"
#include "utils/SimpleAction.hpp"

using namespace mpc::command;

AuditionProgramNoteCommand::AuditionProgramNoteCommand(
    Mpc &mpc, const DrumBusIndex drumBusIndex, const DrumNoteNumber note,
    const Velocity velocity)
    : mpc(mpc), drumBusIndex(drumBusIndex), note(note), velocity(velocity)
{
}

void AuditionProgramNoteCommand::execute()
{
    const auto eventHandler = mpc.getEventHandler();

    mpc.getPerformanceManager().lock()->enqueueCallback(utils::SimpleAction(
        [eventHandler, drumBusIndex = drumBusIndex, note = note,
         velocity = velocity]
        {
            eventHandler->startProgramNoteAudition(drumBusIndex, note,
                                                   velocity);
        }));
}
