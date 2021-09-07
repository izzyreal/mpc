#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <disk/Volume.hpp>

namespace mpc::lcdgui::screens {
class VmpcDisksScreen : public mpc::lcdgui::ScreenComponent
{
public:
    VmpcDisksScreen(mpc::Mpc& mpc, const int layerIndex);
    void open() override;
    void function(int i) override;
    void turnWheel(int i) override;
    void up() override;
    void down() override;
    
private:
    std::map<std::string, mpc::disk::MountMode> config;
    int row = 0;
    int rowOffset = 0;

    void displayRows();
    void displayFunctionKeys();
    void displayUpAndDown();
    
    bool hasConfigChanged();

};
}
