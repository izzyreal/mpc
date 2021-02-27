#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::disk { class AllLoader; }

namespace mpc::lcdgui::screens {
class MidiSwScreen
: public mpc::lcdgui::ScreenComponent
{
    
public:
    MidiSwScreen(mpc::Mpc&, const int layerIndex);
    
    void open() override;
    void turnWheel(int i) override;
    void function(int i) override;
        
    void left() override;
    void right() override;
    std::pair<int, int> getSwitch(int index);
    
private:
    int xOffset = 0;
    
    void setXOffset(int i);
    
    void displaySwitchLabels();
    void displayCtrlsAndFunctions();
    
    void setSwitch(const int index, const std::pair<int, int>);

    void setSwitches(const std::vector<std::pair<int, int>>&);
    
    void initializeDefaultMapping();
    
    const int SWITCH_COUNT = 40;
    std::vector<std::pair<int, int>> controllerToFunctionMapping;
    
    const std::vector<std::string> functionNames{ "PLAY STRT", "PLAY", "STOP", "REC+PLAY", "ODUB+PLAY", "REC/PUNCH", "ODUB/PNCH", "TAP", "PAD BNK A", "PAD BNK B", "PAD BNK C", "PAD BNK D", " PAD   1", " PAD   2", " PAD   3", " PAD   4", " PAD   5", " PAD   6", " PAD   7", " PAD   8", " PAD   9", " PAD  10", " PAD  11", " PAD  12", " PAD  13", " PAD  14", " PAD  15", " PAD  16", "   F1", "   F2", "   F3", "   F4", "   F5", "   F6" };
    
    friend class mpc::disk::AllLoader;
};
}
