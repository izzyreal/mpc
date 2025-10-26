#pragma once

#include "command/Command.hpp"

#include <memory>

namespace mpc::lcdgui
{
    class LayeredScreen;
}

namespace mpc::input
{
    class PadAndButtonKeyboard;
}

namespace mpc::hardware
{
    class Hardware;
}

namespace mpc::controller
{
    class ClientEventController;
}

namespace mpc::command
{
    class PushFullLevelCommand : public Command
    {
    public:
        explicit PushFullLevelCommand(
                std::shared_ptr<lcdgui::LayeredScreen>,
                std::shared_ptr<input::PadAndButtonKeyboard>,
                std::shared_ptr<hardware::Hardware>,
                std::shared_ptr<controller::ClientEventController>);
        void execute() override;

    private:
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<input::PadAndButtonKeyboard> padAndButtonKeyboard;
        std::shared_ptr<hardware::Hardware> hardware;
        std::shared_ptr<controller::ClientEventController> controller;
    };
} // namespace mpc::command
