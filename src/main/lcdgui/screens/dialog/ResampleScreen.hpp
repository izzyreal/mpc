#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{
    class ResampleScreen final : public ScreenComponent, public OpensNameScreen
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;
        void openNameScreen() override;

        ResampleScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        const std::vector<std::string> qualityNames =
            std::vector<std::string>{"LOW", "MED", "HIGH"};
        const std::vector<std::string> bitNames =
            std::vector<std::string>{"16", "12", "8"};

        void displayNewFs() const;
        void displayQuality() const;
        void displayNewBit() const;
        void displayNewName() const;

        void setNewFs(int i);
        void setQuality(int i);
        void setNewBit(int i);
        void setNewName(const std::string &s);

        std::string newName;
        int newFs = 44100;
        int quality = 0;
        int newBit = 0;

        friend class NameScreen;
    };
} // namespace mpc::lcdgui::screens::dialog
