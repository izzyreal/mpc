#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "nvram/MidiControlPersistence.hpp"

#include <vector>
#include <string>
#include <functional>

namespace mpc::lcdgui::screens::window {

    class VmpcKnownControllerDetectedScreen;

class VmpcMidiPresetsScreen
: public mpc::lcdgui::ScreenComponent
{
    
public:
    VmpcMidiPresetsScreen(mpc::Mpc& mpc, const int layerIndex);

    void open() override;
    void function(int i) override;
    void turnWheel(int i) override;
    void up() override;
    void down() override;
    void left() override;
    void right() override;

    int getActivePresetIndex();

private:
    std::function<void(std::string&)> saveMappingAndShowPopup;
    int row = 0;
    int rowOffset = 0;
    unsigned char column = 0;
    std::string activePresetName = "New preset";

    void displayUpAndDown();
    void displayRows();

    std::vector<std::string> autoLoadModeNames{"NO", "ASK", "YES"};

    friend class mpc::lcdgui::screens::window::VmpcKnownControllerDetectedScreen;
};
}
