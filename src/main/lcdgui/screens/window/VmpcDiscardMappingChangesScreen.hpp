#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {
class VmpcKeyboardScreen;
class VmpcMidiScreen;
}

namespace mpc::lcdgui::screens::window {
class VmpcDiscardMappingChangesScreen
: public mpc::lcdgui::ScreenComponent
{
    
public:
    VmpcDiscardMappingChangesScreen(mpc::Mpc& mpc, const int layerIndex);
    
    void function(int i) override;
    void close() override;
    void mainScreen() override {}
    
private:
    std::string nextScreen = "sequencer";
    
    friend class mpc::lcdgui::screens::VmpcKeyboardScreen;
    friend class mpc::lcdgui::screens::VmpcMidiScreen;
};
}
