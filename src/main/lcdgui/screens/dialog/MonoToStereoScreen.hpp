#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{
    class MonoToStereoScreen : public mpc::lcdgui::ScreenComponent,
                               public mpc::lcdgui::screens::OpensNameScreen
    {

    public:
        void turnWheel(int i) override;
        void function(int j) override;
        void openNameScreen() override;

        MonoToStereoScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

    private:
        void displayLSource();
        void displayRSource();
        void displayNewStName();

        std::string newStName;
        int rSource = 0;
        void setRSource(int i);

        friend class mpc::lcdgui::screens::window::NameScreen;
    };
} // namespace mpc::lcdgui::screens::dialog
