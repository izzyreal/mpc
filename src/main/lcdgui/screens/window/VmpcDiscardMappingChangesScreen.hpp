#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {
class VmpcDiscardMappingChangesScreen
: public mpc::lcdgui::ScreenComponent
{
    
public:
    void function(int i) override;
    void mainScreen() override {}
    
    VmpcDiscardMappingChangesScreen(mpc::Mpc& mpc, const int layerIndex);
    
};
}
