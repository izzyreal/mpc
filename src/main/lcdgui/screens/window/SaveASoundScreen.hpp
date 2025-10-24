#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class FileExistsScreen;
}

namespace mpc::lcdgui::screens::window
{
    class SaveASoundScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        SaveASoundScreen(mpc::Mpc &mpc, const int layerIndex);
        void open() override;

    private:
        void setFileType(int i);
        int fileType = 0;
        void displayFileType();
        void displayFile();

        friend class mpc::lcdgui::screens::dialog::FileExistsScreen;
    };
} // namespace mpc::lcdgui::screens::window
