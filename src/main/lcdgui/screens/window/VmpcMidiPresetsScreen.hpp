#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <functional>

namespace mpc::lcdgui::screens {
class VmpcKeyboardScreen;
class VmpcMidiScreen;
}

namespace mpc::lcdgui::screens::window {
class VmpcMidiPresetsScreen
: public mpc::lcdgui::ScreenComponent
{
    
public:
    VmpcMidiPresetsScreen(mpc::Mpc& mpc, const int layerIndex);
    
    void function(int i) override;
};
}
