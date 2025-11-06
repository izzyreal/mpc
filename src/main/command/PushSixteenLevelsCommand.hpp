#pragma once

#include "command/Command.hpp"

#include <memory>

namespace mpc::lcdgui
{
    class LayeredScreen;
}

namespace mpc::controller
{
    class ClientEventController;
}

namespace mpc::hardware
{
    class Hardware;
}

namespace mpc::command
{
    class PushSixteenLevelsCommand : public Command
    {
    public:
        explicit PushSixteenLevelsCommand(
            const std::shared_ptr<lcdgui::LayeredScreen> &,
            const std::shared_ptr<controller::ClientEventController> &,
            const std::shared_ptr<hardware::Hardware> &);
        void execute() override;

    private:
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<controller::ClientEventController> controller;
        std::shared_ptr<hardware::Hardware> hardware;
    };

} // namespace mpc::command
