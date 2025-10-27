#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens
{
    class SaveScreen;
}

namespace mpc::lcdgui::screens::dialog
{
    class FileExistsScreen;
}

namespace mpc::lcdgui::screens::window
{
    class SaveApsFileScreen : public mpc::lcdgui::ScreenComponent,
                              public mpc::lcdgui::screens::OpensNameScreen
    {
    public:
        SaveApsFileScreen(mpc::Mpc &mpc, const int layerIndex);

        void turnWheel(int i) override;
        void function(int i) override;
        void openNameScreen() override;

        void open() override;

    private:
        const std::vector<std::string> apsSaveNames{"APS ONLY", "WITH SOUNDS",
                                                    "WITH .WAV"};

        std::string fileName = "ALL_PGMS";

        void displayFile();
        void displaySave();
        void displayReplaceSameSounds();

        friend class mpc::lcdgui::screens::SaveScreen;
        friend class mpc::lcdgui::screens::dialog::FileExistsScreen;
    };
} // namespace mpc::lcdgui::screens::window
