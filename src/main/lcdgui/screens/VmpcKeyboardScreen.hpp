#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc { class Mpc; }

namespace mpc::lcdgui::screens{

class VmpcKeyboardScreen
: public mpc::lcdgui::ScreenComponent
{

public:
    VmpcKeyboardScreen(mpc::Mpc&, int layerIndex);
    
    void open() override;
    void up() override;
    void down() override;
    void function(int i) override;
    
    
private:
    int row = 0;
    int rowOffset = 0;
    
    void updateRows();
    
};
}
