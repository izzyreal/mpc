#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <string>

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
    void mainScreen() override;
    void turnWheel(int i) override;
    
    bool isLearning();
    void setLearnCandidate(const int rawKeyCode);

private:
    int row = 0;
    int rowOffset = 0;
    int learnCandidate = -1;
    
    bool learning = false;
    bool hasMappingChanged();
    
    void setLearning(bool b);
    void updateRows();
    void displayUpAndDown();
        
};
}
