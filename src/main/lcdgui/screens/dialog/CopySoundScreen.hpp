#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{

    class CopySoundScreen final : public ScreenComponent, public OpensNameScreen
    {
    public:
        void function(int i) override;
        void turnWheel(int renamerNewName) override;

        CopySoundScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void openNameScreen() override;

    private:
        void displaySnd() const;
        void displayNewName() const;

        std::string newName;
        void setNewName(const std::string &s);

        friend class NameScreen;
    };
} // namespace mpc::lcdgui::screens::dialog
