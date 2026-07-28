#include "ReleaseProgramNoteAuditionCommand.hpp"

#include "Mpc.hpp"
#include "audiomidi/EventHandler.hpp"
#include "performance/PerformanceManager.hpp"
#include "utils/SimpleAction.hpp"

using namespace mpc::command;

ReleaseProgramNoteAuditionCommand::ReleaseProgramNoteAuditionCommand(Mpc &mpc)
    : mpc(mpc)
{
}

void ReleaseProgramNoteAuditionCommand::execute()
{
    const auto eventHandler = mpc.getEventHandler();

    mpc.getPerformanceManager().lock()->enqueueCallback(utils::SimpleAction(
        [eventHandler]
        {
            eventHandler->releaseProgramNoteAudition();
        }));
}
