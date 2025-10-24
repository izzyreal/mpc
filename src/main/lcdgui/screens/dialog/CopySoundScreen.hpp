#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{

    class CopySoundScreen
        : public mpc::lcdgui::ScreenComponent,
          public mpc::lcdgui::screens::OpensNameScreen
    {

    public:
        void function(int i) override;
        void turnWheel(int renamerNewName) override;

        CopySoundScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;
        void openNameScreen() override;

    private:
        void displaySnd();
        void displayNewName();

        std::string newName;
        void setNewName(std::string s);

    private:
        friend class mpc::lcdgui::screens::window::NameScreen;
    };
} // namespace mpc::lcdgui::screens::dialog
