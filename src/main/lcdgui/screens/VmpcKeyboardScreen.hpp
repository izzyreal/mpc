#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "input/KeyCodeHelper.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui::screens
{

    class VmpcKeyboardScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        VmpcKeyboardScreen(mpc::Mpc &, int layerIndex);

        void open() override;
        void up() override;
        void down() override;
        void function(int i) override;
        void turnWheel(int i) override;

        bool isLearning();
        void setLearnCandidate(const int rawKeyCode);

        bool hasMappingChanged();

    private:
        int row = 0;
        int rowOffset = 0;
        mpc::input::VmpcKeyCode learnCandidate = mpc::input::VmpcKeyCode::VMPC_KEY_UNKNOWN;

        bool learning = false;
        void setLearning(bool b);
        void updateRows();
        void displayUpAndDown();
    };
} // namespace mpc::lcdgui::screens
