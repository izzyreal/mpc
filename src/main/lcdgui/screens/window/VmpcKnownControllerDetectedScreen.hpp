#pragma once

#include "lcdgui/ScreenComponent.hpp"

namespace mpc::audiomidi { class MidiDeviceDetector; }

namespace mpc::lcdgui::screens::window {
    class VmpcKnownControllerDetectedScreen : public ScreenComponent {
    public:
        VmpcKnownControllerDetectedScreen(mpc::Mpc&, int layer);

        void function(int) override;
        void open() override;

    private:
        void displayMessage();

        std::string controllerName;

        // 0 == disabled, 1 = ask, 2 = enabled
        int shouldAutoSwitch = 1;

        friend class mpc::audiomidi::MidiDeviceDetector;
    };
}