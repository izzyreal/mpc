#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{
    class StereoToMonoScreen final : public ScreenComponent,
                               public OpensNameScreen
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;
        void openNameScreen() override;

        StereoToMonoScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displayStereoSource();
        void displayNewLName();
        void displayNewRName();
        void updateNewNames();

        std::string newLName;
        std::string newRName;

        void setNewLName(const std::string &s);
        void setNewRName(const std::string &s);

        friend class NameScreen;
    };
} // namespace mpc::lcdgui::screens::dialog
