#pragma once

#include "hardware/IHardware.h"

#include <memory>

namespace mpc::lcdgui::screens {
    class IVmpcMidiScreen;
}

namespace mpc::engine::midi { class ShortMessage; }

namespace mpc::audiomidi {

    class VmpcMidiControlMode
    {
    public:
        int previousDataWheelValue = -1;

        void processMidiInputEvent(std::shared_ptr<mpc::hardware::IHardware>,
                                   std::shared_ptr<mpc::lcdgui::screens::IVmpcMidiScreen>,
                                   mpc::engine::midi::ShortMessage* msg);
    };
}
