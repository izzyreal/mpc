#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::nvram { class NvRam; }

namespace mpc::lcdgui::screens
{
class VmpcAutoSaveScreen
: public mpc::lcdgui::ScreenComponent
{

public:
    void turnWheel(int i) override;
    
    VmpcAutoSaveScreen(mpc::Mpc& mpc, const int layerIndex);
    
    void open() override;
    void function(int) override;
    
private:
    const std::vector<std::string> autoSaveOnExitNames{ "Disabled", "Ask", "Enabled" };
    const std::vector<std::string> autoLoadOnStartNames{ "Disabled", "Ask", "Enabled" };
    
    int autoSaveOnExit = 2;
    int autoLoadOnStart = 1;
    
    void displayAutoSaveOnExit();
    void displayAutoLoadOnStart();

    friend class mpc::nvram::NvRam;

public:
    void setAutoSaveOnExit(int i);
    void setAutoLoadOnStart(int i);
    
    int getAutoSaveOnExit();
    int getAutoLoadOnStart();

};
}
