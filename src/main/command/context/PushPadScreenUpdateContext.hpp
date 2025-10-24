#pragma once

#include <memory>
#include <functional>

namespace mpc::lcdgui
{
    class ScreenComponent;
}

namespace mpc::sequencer
{
    class Sequencer;
}
namespace mpc::sampler
{
    class Program;
}

namespace mpc::command::context
{

    struct PushPadScreenUpdateContext
    {
        bool isSixteenLevelsEnabled;
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screenComponent;
        std::shared_ptr<sampler::Program> program;
        std::shared_ptr<sequencer::Sequencer> sequencer;
        bool isF4Pressed;
        bool isF6Pressed;
        int bank;
        std::function<void(int)> setMpcPad;
        bool isAllowCentralNoteAndPadUpdateScreen;
    };

} // namespace mpc::command::context
