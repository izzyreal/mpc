#pragma once

#include "lcdgui/ScreenComponent.hpp"

namespace mpc::audiomidi
{
    class MidiDeviceDetector;
}

namespace mpc::lcdgui::screens::window
{
    class VmpcKnownControllerDetectedScreen : public ScreenComponent
    {
    public:
        VmpcKnownControllerDetectedScreen(mpc::Mpc &, int layer);

        void function(int) override;
        void open() override;

    private:
        void displayMessage();

        std::string controllerName;

        friend class mpc::audiomidi::MidiDeviceDetector;
    };
} // namespace mpc::lcdgui::screens::window