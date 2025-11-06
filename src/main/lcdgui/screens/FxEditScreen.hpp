#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui
{
    class Effect;
}

namespace mpc::lcdgui::screens
{
    class FxEditScreen final : public ScreenComponent
    {

    public:
        void function(int f) override;
        void turnWheel(int increment) override;
        void left() override;
        void right() override;
        void up() override;
        void down() override;

    private:
        void checkEffects() const;
        std::vector<std::weak_ptr<Effect>> findEffects() const;

    public:
        FxEditScreen(Mpc &mpc, int layerIndex);
        void open() override;

    private:
        void displayDrum() const;
        void displayPgm() const;
        void displayEdit() const;
    };
} // namespace mpc::lcdgui::screens
