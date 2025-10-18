#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {
class VmpcResetKeyboardScreen
: public mpc::lcdgui::ScreenComponent
{
    
public:
    void function(int i) override;
    
    VmpcResetKeyboardScreen(mpc::Mpc& mpc, const int layerIndex);
    
};
}
