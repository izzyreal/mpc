#include "PushBankCommand.hpp"

#include "controller/ClientEventController.hpp"

using namespace mpc::controller;

namespace mpc::command
{

    PushBankCommand::PushBankCommand(
        std::shared_ptr<ClientEventController> controllerToUse,
        const Bank bankToUse)
        : controller(controllerToUse), bank(bankToUse)
    {
    }

    void PushBankCommand::execute()
    {
        controller->setActiveBank(bank);
    }

} // namespace mpc::command
