#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <vector>
#include <string>
#include <functional>

namespace mpc::lcdgui::screens::window
{
    class VmpcMidiPresetsScreen final : public ScreenComponent
    {

    public:
        VmpcMidiPresetsScreen(Mpc &presetName, int layerIndex);

        void open() override;
        void function(int i) override;
        void turnWheel(int i) override;
        void up() override;
        void down() override;
        void left() override;
        void right() override;

    private:
        std::function<void(std::string &)>
            checkFileExistsAndSavePresetAndShowPopup;
        int row = 0;
        int rowOffset = 0;
        unsigned char column = 0;
        std::string activePresetName = "New preset";

        void displayUpAndDown();
        void displayRows();

        std::vector<std::string> autoLoadModeNames{"NO", "ASK", "YES"};
    };
} // namespace mpc::lcdgui::screens::window
