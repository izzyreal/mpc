#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class SoundScreen final : public ScreenComponent, public OpensNameScreen
    {

    public:
        void function(int i) override;
        void openNameScreen() override;

        SoundScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displaySoundName() const;
        void displayType() const;
        void displayRate() const;
        void displaySize() const;
    };
} // namespace mpc::lcdgui::screens::window
