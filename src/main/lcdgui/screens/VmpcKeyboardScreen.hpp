#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "input/keyboard/KeyboardBindings.hpp"
#include "input/keyboard/VmpcKeyCode.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui::screens
{

    class VmpcKeyboardScreen final : public ScreenComponent
    {

    public:
        VmpcKeyboardScreen(Mpc &, int layerIndex);

        void open() override;
        void up() override;
        void down() override;
        void function(int i) override;
        void turnWheel(int increment) override;

        bool isLearning() const;
        void setLearnCandidate(int platformKeyCode);

        bool hasMappingChanged() const;

    private:
        int row = 0;
        int rowOffset = 0;
        input::keyboard::VmpcKeyCode learnCandidate =
            input::keyboard::VmpcKeyCode::VMPC_KEY_UNKNOWN;

        bool learning = false;

        std::unique_ptr<input::keyboard::KeyboardBindings> bindings;

        void setLearning(bool b);
        void updateRows();
        void displayUpAndDown();
    };
} // namespace mpc::lcdgui::screens
