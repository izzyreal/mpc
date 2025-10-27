#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui
{
    class Effect;
}

namespace mpc::lcdgui::screens
{
    class FxEditScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int f) override;
        void turnWheel(int increment) override;
        void left() override;
        void right() override;
        void up() override;
        void down() override;

    private:
        void checkEffects();
        std::vector<std::weak_ptr<Effect>> findEffects();

    public:
        FxEditScreen(mpc::Mpc &mpc, const int layerIndex);
        void open() override;

    private:
        void displayDrum();
        void displayPgm();
        void displayEdit();
    };
} // namespace mpc::lcdgui::screens
