#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window {
class VmpcMidiPresetsScreen
: public mpc::lcdgui::ScreenComponent
{
    
public:
    VmpcMidiPresetsScreen(mpc::Mpc& mpc, const int layerIndex);

    void open() override;
    void function(int i) override;
    void up() override;
    void down() override;

    void displayUpAndDown();
    void displayRows();

private:
    int row = 0;
    int rowOffset = 0;

    void initPresets();

    std::vector<std::string> presets;
};
}
