#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <thread>
#include <memory>

namespace mpc::lcdgui
{
    class Label;
}

namespace mpc::lcdgui::screens::dialog
{

    class MidiInputMonitorScreen final : public ScreenComponent
    {

        std::thread blinkThread;
        void runBlinkThread(const std::weak_ptr<Label> &label) const;
        static void static_blink(void *args, const std::weak_ptr<Label> &label);

    public:
        void initTimer(std::weak_ptr<Label> label);
        void update(Observable *o, Message message) override;

        MidiInputMonitorScreen(Mpc &mpc, int layerIndex);
        ~MidiInputMonitorScreen() override;

        void open() override;
        void close() override;
    };
} // namespace mpc::lcdgui::screens::dialog
