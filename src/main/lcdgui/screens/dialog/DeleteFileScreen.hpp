#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::dialog
{
    class DeleteFileScreen final : public ScreenComponent
    {
    public:
        DeleteFileScreen(Mpc &mpc, int layerIndex);
        void function(int i) override;

    private:
        void deleteFile();
    };
} // namespace mpc::lcdgui::screens::dialog
