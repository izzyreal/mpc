#pragma once

#include "command/Command.hpp"

#include "controller/Bank.hpp"

#include <memory>

namespace mpc::controller
{
    class ClientEventController;
}

namespace mpc::command
{

    class PushBankCommand : public Command
    {
    public:
        PushBankCommand(std::shared_ptr<controller::ClientEventController>,
                        const controller::Bank);
        void execute() override;

    private:
        std::shared_ptr<controller::ClientEventController> controller;
        const controller::Bank bank;
    };

} // namespace mpc::command
