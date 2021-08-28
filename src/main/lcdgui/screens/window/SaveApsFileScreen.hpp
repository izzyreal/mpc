#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {
class SaveScreen;
}

namespace mpc::lcdgui::screens::window {
    class NameScreen;
}

namespace mpc::lcdgui::screens::window
{
class SaveApsFileScreen
: public mpc::lcdgui::ScreenComponent
{
public:
    SaveApsFileScreen(mpc::Mpc& mpc, const int layerIndex);
    
    void turnWheel(int i) override;
    void function(int i) override;
    
    void open() override;
    
private:
    const std::vector<std::string> apsSaveNames{ "APS ONLY", "WITH SOUNDS", "WITH .WAV" };
    
    std::string fileName = "ALL_PROGRAM";
    
    void displayFile();
    void displaySave();
    void displayReplaceSameSounds();
    
    friend class NameScreen;
    friend class mpc::lcdgui::screens::SaveScreen;
};
}
