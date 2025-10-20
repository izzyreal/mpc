#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class DeleteFileScreen : public mpc::lcdgui::ScreenComponent
    {
    public:
        DeleteFileScreen(mpc::Mpc& mpc, int layerIndex);
        void function(int i) override;
    private:
        void deleteFile();

    };
}

