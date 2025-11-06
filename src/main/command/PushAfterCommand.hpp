#pragma once

#include "command/Command.hpp"

#include <memory>

namespace mpc::controller
{
    class ClientEventController;
}

namespace mpc::hardware
{
    class Hardware;
}

namespace mpc::lcdgui
{
    class LayeredScreen;
}

namespace mpc::command
{

    class PushAfterCommand : public Command
    {
    public:
        explicit PushAfterCommand(
            const std::shared_ptr<controller::ClientEventController> &,
            const std::shared_ptr<lcdgui::LayeredScreen> &,
            const std::shared_ptr<hardware::Hardware> &);

        void execute() override;

    private:
        std::shared_ptr<controller::ClientEventController> controller;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<hardware::Hardware> hardware;
    };

} // namespace mpc::command
