#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens
{
    class SaveScreen;
}

namespace mpc::lcdgui::screens::window
{
    class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{
    class FileExistsScreen;
}

namespace mpc::lcdgui::screens::window
{
    class SaveAllFileScreen : public mpc::lcdgui::ScreenComponent,
                              public mpc::lcdgui::screens::OpensNameScreen
    {

    public:
        void function(int i) override;
        void openNameScreen() override;
        void open() override;

        SaveAllFileScreen(mpc::Mpc &mpc, const int layerIndex);

    private:
        void displayFile();
        std::string fileName = "ALL_SEQ_SONG1";

        friend class NameScreen;
        friend class mpc::lcdgui::screens::SaveScreen;
        friend class mpc::lcdgui::screens::dialog::FileExistsScreen;
    };
} // namespace mpc::lcdgui::screens::window
